#!/bin/bash

echo "<html>"
echo "<head>"
echo "<h2>Environment with BASH</h2><br>"
echo "</head>"
echo "<body>"
# Loop through all environment variables and echo them
for va in $(env | cut -d '=' -f1); do
    echo "$va=${!va}<br>"
done
echo "</body>" 
echo "</html>" 