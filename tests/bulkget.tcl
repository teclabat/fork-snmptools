#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."
set ret [snmp session lo -v2c -Os -c public localhost]

puts "{[lo bulkget -h]}"

puts "Getting.."
set ret [lo bulkget -Cn1 -Cr5 system ifTable]
puts "Result: $ret"
puts "errorInfo: \n{$errorInfo}"

puts "Closing.."
lo close
