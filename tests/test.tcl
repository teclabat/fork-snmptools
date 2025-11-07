#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."

puts {----------------------------------------------------------------------------------}

catch {set ret [snmp session];}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
if {[lsearch -exact [info commands] lo] >= 0} {puts Created}

puts {----------------------------------------------------------------------------------}

catch {set ret [snmp session lo]}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
if {[lsearch -exact [info commands] lo] >= 0} {puts Created}

puts {----------------------------------------------------------------------------------}

catch {set ret [snmp session lo asdf]}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
if {[lsearch -exact [info commands] lo] >= 0} {puts Created}

puts {----------------------------------------------------------------------------------}

catch {set ret [snmp session lo -v1 -c public localhost]}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
if {[lsearch -exact [info commands] lo] >= 0} {puts Created}

puts {----------------------------------------------------------------------------------}

catch {set ret [lo get]}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}

puts {----------------------------------------------------------------------------------}

catch {set ret [lo get sysLocation.0]}
puts "errorInfo: {$errorInfo}"; set errorInfo {}
catch {puts "Result: $ret";}
puts "errorInfo: {$errorInfo}"; set errorInfo {}

exit
puts {----------------------------------------------------------------------------------}

puts "Closing.."
lo close
