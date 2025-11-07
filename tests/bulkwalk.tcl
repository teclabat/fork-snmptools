#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."
set ret [snmp session lo -v2c -Ov -Oe -Oq -OU -Ot -c public localhost]

puts "Walking.."
set ret [lo bulkwalk system]
puts "Result: $ret"
puts "errorInfo: \n$errorInfo"

puts "Closing.."
lo close
