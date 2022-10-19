
## rt5350_GPL_source.tar.gz

Due to GitHub's file size limit the file was split into chunks:

~~~sh
$ split --verbose -b 50M -d -a 1 rt5350_GPL_source.tar.gz rt5350_GPL_source.tar.gz.
creating file 'rt5350_GPL_source.tar.gz.0'
creating file 'rt5350_GPL_source.tar.gz.1'
creating file 'rt5350_GPL_source.tar.gz.2'
creating file 'rt5350_GPL_source.tar.gz.3'
~~~

Re-assemble with:

~~~sh
$ cat rt5350_GPL_source.tar.gz.* > rt5350_GPL_source.tar.gz
~~~
