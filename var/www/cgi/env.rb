#!/usr/bin/ruby

puts("<html>")

puts("<head>")
puts("<h2>Environment with RUBY</h2><br>")
puts("</head>")

puts("<body>")
# Loop through all environment variables and print them
ENV.each do |key, value|
  puts "#{key}=#{value}<br>"
end
puts("</body>")

puts("</html>")