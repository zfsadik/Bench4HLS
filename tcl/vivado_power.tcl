# vivado_power.tcl: run power flow and write power_out/{design}.txt

# 1) Get design name
if {[llength $argv] != 1} {
    puts stderr "ERROR: missing design name"
    exit 1
}
set design [lindex $argv 0]
puts ">> Vivado power for $design"

# 2) Paths
set root   [file normalize [pwd]]
set ddir   [file join $root $design]
set vdir   [file join $ddir synthesis syn verilog]
set odir   [file join $ddir vivado_power]
file mkdir $odir

# 3) Output dir for per-design power result (safe for parallel execution)
set powerDir [file join $root power_out]
file mkdir $powerDir

# 4) Check for RTL
if {![file exists $vdir] || [llength [glob -directory $vdir *.v]] == 0} {
    puts " SKIP: no RTL under $vdir"
    set fh [open [file join $powerDir "${design}.txt"] w]
    puts $fh "N/A"
    close $fh
    exit 0
}

# 5a) Read all Verilog files
foreach f [glob -directory $vdir *.v] {
    read_verilog -sv [file join $vdir $f]
}

# 5b) Source any IP instantiation .tcl scripts
foreach ip_tcl [glob -nocomplain -directory $vdir *_ip.tcl] {
    puts " Sourcing IP TCL: $ip_tcl"
    source $ip_tcl
}

# 5c) Upgrade and generate targets for IPs (if any)
if {[llength [get_ips *]] > 0} {
    upgrade_ip [get_ips *]
    generate_target all [get_ips *]
}

# 6) Try synthesis + power estimation
set PWR "ERROR"
if {[catch {
    synth_design -top TopModule -part xc7a200tffv1156-1
    opt_design

    # Power report
    set rpt [file join $odir TopModule_power.rpt]
    report_power -file $rpt

    # Parse power value from report
    set fp [open $rpt r]
    set txt [read $fp]
    close $fp

    foreach line [split $txt "\n"] {
        set line [string trimright $line "\r"]
        if {[regexp {\|\s*Total On-Chip Power \(W\)\s*\|\s*([0-9.]+)} $line -> val]} {
            set PWR $val
            break
        }
    }

    puts "   -> Power(W) = $PWR"
} result]} {
    puts " ERROR: Vivado failed on $design — $result"
}

# 7) Write power result to per-design file
set fh [open [file join $powerDir "${design}.txt"] w]
puts $fh $PWR
close $fh

exit 0
