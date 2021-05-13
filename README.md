<div class="output_wrapper" id="output_wrapper_id"><h3 id="hpasswordcheckenhanceforpostgresql"><span>passwordcheck-enhance for PostgreSQL</span></h3>
<h3 id="habouthowtouse"><span>About how to use</span></h3>
<ol>
<li><p>You need to download two files in this repository,then copy to "src package"'s contrib directory from your download directory</p></li>
<li><p>Switch to contrib directory and execute make command</p></li>
<li><p>You need to copy passwordcheck-enhance.so file to lib's install path of PostgreSQL as "/data/pg12.6/lib/postgresql" </p></li>
<li><p>You need add shared_preload_libraries = 'passwordcheck-enhance' item to PGDATA/postgresql.auto.conf or PGDATA/postgresql.conf file</p></li>
<li><p>Restart your PostgreSQL server.</p></li>
</ol>
<h3 id="hexmalple"><span>Exmalple</span></h3>

```
postgres=# create user u1 with password 'u1';
ERROR:  密码长度至少需要 8 位，并且至少需要包含一个大小写字母和特殊字符 
postgres=# create user u1 with password 'u1A';
ERROR:  密码长度至少需要 8 位，并且至少需要包含一个大小写字母和特殊字符 
postgres=# create user u1 with password 'earearear';
ERROR:  密码至少需要包含 1 个大写字母
postgres=# create user u1 with password 'eareareaA';
ERROR:  密码至少需要包含 1 个数字
postgres=# create user u1 with password 'eareareaA1';
ERROR:  密码至少需要包含 1 个特殊字符
postgres=# create user u1 with password 'eareareaA1_';
CREATE ROLE
```
