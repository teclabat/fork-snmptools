#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."
set ret [snmp session lo -v1 -c private localhost]

puts "Setting.."
puts "Old:[lo get sysName.0]"
lo set sysName.0 s testname
puts "New:[lo get sysName.0]"
puts "errorInfo: \n$errorInfo"

puts "Closing.."
lo close
