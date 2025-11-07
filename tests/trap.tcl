#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

set errorInfo {}
puts "Sending trap.."
catch {set ret [snmp trap -v 1 -c public udp:localhost:161 "" "" 0 0  ""]}
puts "errorInfo: \n{ $errorInfo }"
catch {puts "Result: $ret"}

puts {-----------------------------}

set errorInfo {}
puts "Sending trap.."
catch {set ret [snmp trap -v 2c -c public localhost "" ucdStart sysContact.0 s "Dave"]}
puts "errorInfo: \n{ $errorInfo }"
catch {puts "Result: $ret"}

puts {-----------------------------}

puts "Sending trap.."
catch {set ret [snmp trap localhost "" ucdStart sysContact.0 s "Dave"]}
puts "errorInfo: \n{ $errorInfo }"
catch {puts "Result: $ret"}

puts {-----------------------------}

set errorInfo {}
puts "Sending trap.."
catch {set ret [snmp trap -v 1 -c public udp:localhost:161 "" "" 0 0  ""]}
puts "errorInfo: \n{ $errorInfo }"
catch {puts "Result: $ret"}

