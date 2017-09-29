# PocketSphinxTCPServer

Simple speech recognition multithread server written on Qt and pocketsphinx

# Compilation
qmake
make 
sudo make install

# Start
example start args:
/usr/local/bin/pstcpserver/PocketSphinxTCPServer -upperf 4000 -samprate 8000 -hmm ~/zero_ru/zero_ru.cd_semi_4000 -dict ~/zero_ru/ru.dic -lm ~/zero_ru/ru.lm -remove_noise no 

after success test launches add args to remove detail verbose:
-logfn /dev/null

go to sphinx doc for detail args description
