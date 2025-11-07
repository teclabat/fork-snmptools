#!/bin/tcl
#
lappend auto_path [pwd]/lib
package require snmptools

puts "Opening.."
set ret [snmp session lo -v2c -c public -OX localhost]

puts "Getting.."
set ret [lo table -Cl -CB -Ci -Cb -Cc 80 -Cw 80 ifTable] 
puts "Result: >>>>>>>>>>>>>>>\n$ret\n<<<<<<<<<<<<<<<<<<<<"
puts "errorInfo: \n{ $errorInfo }"

puts {---------------------}

set ret [lo table -Cl -CB -Cb -Cc 80 -Cw 80 ifTable] 
puts "Result: >>>>>>>>>>>>>>>\n$ret\n<<<<<<<<<<<<<<<<<<<<"
puts "errorInfo: \n{ $errorInfo }"

puts {---------------------}

set ret [lo table -Cl -CB -Ch -Cb -Cc 80 -Cw 80 ifTable] 
puts "Result: >>>>>>>>>>>>>>>\n$ret\n<<<<<<<<<<<<<<<<<<<<"
puts "errorInfo: \n{ $errorInfo }"

puts {---------------------}

set ret [lo table -Cl -CB -CH -Cb -Cc 80 -Cw 80 ifTable] 
puts "Result: >>>>>>>>>>>>>>>\n$ret\n<<<<<<<<<<<<<<<<<<<<"
puts "errorInfo: \n{ $errorInfo }"

puts "Closing.."
lo close
