#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."
set ret [snmp session lo -v1 -c public localhost]

puts "Getting.."
set ret [lo get sysLocation.0 sysName.0 system.sysUpTime.0 system.sysContact.0]
puts "Result: $ret"
puts "errorInfo: \n$errorInfo"

puts "Closing.."
lo close
