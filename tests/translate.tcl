#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

set errorInfo {}
puts "Translating.."
catch {set ret [snmp translate]}
puts "errorInfo: \n{$errorInfo}"
catch {puts "Result: $ret"}

puts {------------------------}

set errorInfo {}
puts "Translating.."
catch {set ret [snmp translate -Onf -IR sysDescr]}
puts "errorInfo: \n{$errorInfo}"
catch {puts "Result: $ret"}
