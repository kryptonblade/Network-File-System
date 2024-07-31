# final-project-54
final-project-54 created by GitHub Classroom

## Input Format
<p><b>READ : </b>READ < rel_file_path ></p>
<p><b>WRITE : </b>WRITE < rel_file_path ></p>
<p><b>FILEINFO : </b>FILEINFO < rel_file_path ></p>
<p><b>CREATE : </b>CREATE < rel_file_path ( OR ) rel_dir_path ></p>
<p><b>DELETE : </b>DELETE < rel_file_path ( OR ) rel_dir_path ></p>
<p><b>COPY : </b></p>
<p>1.in same storage_server : COPY < rel_file_path_source > < rel_dir_path_dest > < file/dir ></p>
<p>2.in between 2 storage_servers : COPY < rel_file_path_source > < rel_dir_path_dest > </p>

## Assumptions
<p>1.For COPY,there should be no dir with the same name as source file at the destination.</p>
<p>2.File or Folder Names contain only lowercase alphabets.</p>
