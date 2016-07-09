require 'fileutils'

task default: :copy_changed_files

desc 'Copy changed source files from ORZ_TOP'
task :copy_changed_files do
  fail 'Environment variable ORZ_TOP must be set' if ENV['ORZ_TOP'].nil?
  orz_top = ENV['ORZ_TOP'].gsub('\\', '/')
  fail 'ORZ_TOP not set' unless orz_top
  dir_name = 'changed_source_files'
  mkdir(dir_name) unless File.directory?(dir_name)

  temp_dir = 'temp'
  mkdir(temp_dir) unless File.directory?(temp_dir)

  src_dir = File.join(orz_top, 'src', 'Libs', 'OrzJsonRpc')

  Dir[File.join(src_dir, '*.{cpp,h}')]
    .reject { |n| n =~ /pch/ }
    .reject { |n| n =~ /jcon_assert\.h/}.each do |file|
    basename = File.basename(file)
    dest_file = File.join(temp_dir, basename)
    FileUtils.cp(file, dest_file)
    system(%Q[sed -i 'N;N;s/#include "pch\.h"\\n\\n//' #{dest_file}])
  end

  Dir[File.join(temp_dir, '*')].each do |file|
    basename = File.basename(file)
    dest_file = File.join('src', 'jcon', basename)

    copy = if !File.exist?(dest_file)
             true
           elsif FileUtils.compare_file(file, dest_file)
             false
           else
             true
           end

    if copy
      FileUtils.copy(file, dest_file, verbose: true)
    end
  end

  FileUtils.rm_rf(temp_dir)
end
