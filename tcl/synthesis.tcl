# synthesis.tcl: run csynth and write synth_out/{design}.txt

# 1) Read environment
foreach v {DESIGN_K DESIGN_RUN DESIGN_START DESIGN_END} {
    if {![info exists ::env($v)]} {
        error "$v not set"
    }
}
set K        $::env(DESIGN_K)
set RUN      $::env(DESIGN_RUN)
set startIdx $::env(DESIGN_START)
set endIdx   $::env(DESIGN_END)
puts "DEBUG: K=$K RUN=$RUN START=$startIdx END=$endIdx"

# 2) Count total problems from run1
set run1files [lsort -dictionary [glob -nocomplain *_design_run1.cpp]]
if {[llength $run1files]==0} { error "No *_design_run1.cpp files" }
set totalProb [llength $run1files]
if {$endIdx > $totalProb} { set endIdx $totalProb }

# 3) Output dir (one file per design, safe for parallel execution)
file mkdir synth_out
proc write_synth_result {designNm synthStatus latency ff lut dsp bram} {
    set fh [open "synth_out/${designNm}.txt" w]
    puts $fh $synthStatus
    puts $fh $latency
    puts $fh $ff
    puts $fh $lut
    puts $fh $dsp
    puts $fh $bram
    close $fh
}

# 4) Gather this slice of design files
set allRunFiles [lsort -dictionary [glob -nocomplain *_design_run${RUN}.cpp]]
set subRuns     [lrange $allRunFiles [expr {$startIdx-1}] [expr {$endIdx-1}]]

# 5) Loop over each design in this slice
foreach srcFile $subRuns {
    regexp {^(.+?)_design_run([0-9]+)\.cpp$} $srcFile -> prefix runNo
    set designNm "${prefix}_run${runNo}"

    # Read simulation status from per-design sim result file
    set simStatus ""
    set simFile "sim_out/${designNm}.txt"
    if {[file exists $simFile]} {
        set fhSim [open $simFile r]
        gets $fhSim compLine   ;# line 1: comp status
        gets $fhSim simLine    ;# line 2: sim status
        close $fhSim
        set simStatus $simLine
    } else {
        set simStatus "FAIL"
    }

    # 5.1) Skip synthesis if simulation failed
    if {$simStatus eq "FAIL"} {
        write_synth_result $designNm FAIL "" "" "" "" ""
        continue
    }

    # Clean & open project/solution
    open_project   "${designNm}"
    open_solution  -reset synthesis
    set_part       {xc7a200tffv1156-1}
    create_clock   -period 10 -name default
    add_files      $srcFile
    set_top        TopModule

    # 5.2) Run synthesis
    if {[catch { csynth_design } rc]} {
        write_synth_result $designNm FAIL "" "" "" "" ""
    } else {
        set synthStatus PASS

        # 5.3) Extract Latency (prefer csynth.xml; fallback to csynth.rpt)
        set LAT 0.0
        set latFound 0

        # 5.3.a) Stable parser from csynth.xml
        set xpt [file normalize "$designNm/synthesis/syn/report/csynth.xml"]
        if {[file exists $xpt]} {
            set xdata [read [open $xpt r]]
            if {[regexp {(?s)<SummaryOfOverallLatency>.*?<Worst-caseRealTimeLatency>[ \t]*([0-9]+(?:\.[0-9]+)?)[ \t]*ns[ \t]*</Worst-caseRealTimeLatency>.*?</SummaryOfOverallLatency>} $xdata -> wclat]} {
                set LAT [expr {double($wclat)}]
                set latFound 1
            } elseif {[regexp {(?s)<Module>[ \t\n\r]*<Name>TopModule</Name>.*?<Worst-caseRealTimeLatency>[ \t]*([0-9]+(?:\.[0-9]+)?)[ \t]*ns[ \t]*</Worst-caseRealTimeLatency>} $xdata -> wclat2]} {
                set LAT [expr {double($wclat2)}]
                set latFound 1
            }
        }

        # 5.3.b) Fallback legacy parser from csynth.rpt
        if {!$latFound} {
            set rpt [file normalize "$designNm/synthesis/syn/report/csynth.rpt"]
            if {[file exists $rpt]} {
                set lines [split [read [open $rpt r]] "\n"]
                set start -1; set end -1
                for {set i 0} {$i < [llength $lines]} {incr i} {
                    if {$start < 0 && [string first "PS: '+' for module; 'o' for loop; '*' for dataflow" [lindex $lines $i]] >= 0} {
                        set start $i
                    }
                    if {$start >= 0 && [string first "== HW Interfaces" [lindex $lines $i]] >= 0} {
                        set end $i
                        break
                    }
                }
                if {$start >= 0 && $end > $start} {
                    set nsColIdx -1
                    for {set h [expr {$start+3}]} {$h < $end} {incr h} {
                        if {[string first "(ns)" [lindex $lines $h]] >= 0} {
                            set hdrs {}
                            foreach col [split [lindex $lines $h] "|"] {
                                lappend hdrs [string trim $col]
                            }
                            set nsColIdx [lsearch -exact $hdrs "(ns)"]
                            break
                        }
                    }
                    if {$nsColIdx >= 0} {
                        set topNs "-"
                        for {set j [expr {$start+3}]} {$j < $end} {incr j} {
                            set row [lindex $lines $j]
                            if {[regexp {^\s*\|\s*\+\s*TopModule\b} $row]} {
                                set fields [split $row "|"]
                                set rawTop [string trim [lindex $fields $nsColIdx]]
                                if {$rawTop ne "-"} { set topNs $rawTop }
                                break
                            }
                        }
                        if {$topNs ne "-"} {
                            set LAT [double($topNs)]
                        } else {
                            for {set j [expr {$start+3}]} {$j < $end} {incr j} {
                                set row [lindex $lines $j]
                                if {[regexp {^\s*\|\s*\+\s*} $row] && ![regexp {^\s*\|\s*\+\s*TopModule\b} $row]} {
                                    set fields [split $row "|"]
                                    set raw [string trim [lindex $fields $nsColIdx]]
                                    if {$raw ne "-"} {
                                        set LAT [expr {$LAT + double($raw)}]
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        # 5.4) Extract Utilization from JSON
        set jf [file normalize "$designNm/synthesis/synthesis_data.json"]
        set FF_UTIL   0
        set LUT_UTIL  0
        set DSP_UTIL  0
        set BRAM_UTIL 0
        if {[file exists $jf]} {
            set data [read [open $jf r]]
            set a [lindex [regexp -all -inline {\"FF\"\s*:\s*\"([0-9]+)\"} $data] end]
            set b [lindex [regexp -all -inline {\"AVAIL_FF\"\s*:\s*\"([0-9]+)\"} $data] end]
            set FF_UTIL   [format "%.4f" [expr {100.0*double($a)/double($b)}]]
            set a [lindex [regexp -all -inline {\"LUT\"\s*:\s*\"([0-9]+)\"} $data] end]
            set b [lindex [regexp -all -inline {\"AVAIL_LUT\"\s*:\s*\"([0-9]+)\"} $data] end]
            set LUT_UTIL  [format "%.4f" [expr {100.0*double($a)/double($b)}]]
            set a [lindex [regexp -all -inline {\"DSP\"\s*:\s*\"([0-9]+)\"} $data] end]
            set b [lindex [regexp -all -inline {\"AVAIL_DSP\"\s*:\s*\"([0-9]+)\"} $data] end]
            set DSP_UTIL  [format "%.4f" [expr {100.0*double($a)/double($b)}]]
            set a [lindex [regexp -all -inline {\"BRAM_18K\"\s*:\s*\"([0-9]+)\"} $data] end]
            set b [lindex [regexp -all -inline {\"AVAIL_BRAM\"\s*:\s*\"([0-9]+)\"} $data] end]
            set BRAM_UTIL [format "%.4f" [expr {100.0*double($a)/double($b)}]]
        }

        write_synth_result $designNm $synthStatus $LAT $FF_UTIL $LUT_UTIL $DSP_UTIL $BRAM_UTIL
    }

    catch { close_project }
}

quit
