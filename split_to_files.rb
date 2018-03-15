#!/usr/bin/ruby
require 'thread'

if ARGV.length != 1 then
	puts "Pass max-amount of threads!"
	exit
end

thread_count = ARGV[0].to_i
arr = []

STDIN.each_line do |line|
	thread = Thread.new do
		tempfile = `tempfile`
		tempfile = tempfile.chomp

		File.delete(tempfile)
		system("mkfifo #{tempfile}")
		puts "#{tempfile}"
		File.open(tempfile, 'w') { |file| file.write(line)  }
	end
	arr << thread

	while arr.length >= thread_count do
		arr = arr.select{|t| t.status }
		if arr.length >= thread_count then
			sleep(0.001)
		end
	end
end

arr.each do |t|
	t.join
end
