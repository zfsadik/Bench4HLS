# simulation.tcl: run csim and write sim_out/{design}.txt

# 1) Read env vars
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

# 2) Count problems via run1 files
set run1files [lsort -dictionary [glob -nocomplain *_design_run1.cpp]]
if {[llength $run1files]==0} { error "No *_design_run1.cpp files" }
set totalProb [llength $run1files]
if {$endIdx > $totalProb} { set endIdx $totalProb }

puts "Pass@${K}: run ${RUN}, problems ${startIdx}–${endIdx}/${totalProb}"

# 3) Write sim result to an isolated per-design file (safe for parallel runs)
file mkdir sim_out
proc write_sim_result {designNm compStatus simStatus message} {
    set fh [open "sim_out/${designNm}.txt" w]
    puts $fh $compStatus
    puts $fh $simStatus
    puts $fh $message
    close $fh
}

# 4) Simulate this run slice
set allRunFiles [lsort -dictionary [glob -nocomplain *_design_run${RUN}.cpp]]
set subRuns     [lrange $allRunFiles [expr {$startIdx-1}] [expr {$endIdx-1}]]

foreach srcFile $subRuns {
    regexp {^(.+?)_design_run([0-9]+)\.cpp$} $srcFile -> prefix runNo
    set tbFile   "${prefix}_tb.cpp"
    set designNm "${prefix}_run${runNo}"

    if {![file exists $tbFile]} {
        write_sim_result $designNm FAIL FAIL "Missing testbench"
        continue
    }

    file delete -force "$designNm"
    open_project   -reset "$designNm"
    set_top        TopModule
    add_files      $srcFile
    add_files -tb  $tbFile
    open_solution  -reset simulation
    create_clock   -period 10 -name default

    catch { csim_design -clean }
    set compRc [catch { csim_design } rc]

    if {$compRc != 0} {
        write_sim_result $designNm FAIL FAIL "csim_design failed: compilation error(s)"
    } else {
        set logFile [file normalize "${designNm}/simulation/csim/report/TopModule_csim.log"]
        if {![file exists $logFile]} {
            set logFile [file normalize "${designNm}/simulation/csim/report/Topmodule_csim.log"]
        }
        if {![file exists $logFile]} {
            write_sim_result $designNm PASS FAIL "Missing TopModule_csim.log"
        } else {
            set fh2 [open $logFile r]
            set lines {}
            while {[gets $fh2 line] >= 0} { lappend lines $line }
            close $fh2

            set statusLine ""
            for {set i [expr {[llength $lines]-1}]} {$i >= 0} {incr i -1} {
                set cur [lindex $lines $i]
                if {[string match "*Test Passed*" $cur] || [string match "*Test Failed*" $cur]} {
                    set statusLine $cur
                    break
                }
            }

            if {$statusLine eq ""} {
                write_sim_result $designNm PASS FAIL "No pass/fail marker in csim log"
            } elseif {[string match "*Test Passed*" $statusLine]} {
                write_sim_result $designNm PASS PASS $statusLine
            } elseif {[string match "*Test Failed*" $statusLine]} {
                write_sim_result $designNm PASS FAIL $statusLine
            } else {
                write_sim_result $designNm PASS FAIL "Unrecognized status: $statusLine"
            }
        }
    }

    catch { close_project }
}

quit
