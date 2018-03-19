# make install is required in order to put the library to the system
# ../ffmpeg -y -i "concat:0.ts|1.ts" -c copy a.ts
#../ffmpeg -y -i "ndn:0.ts|1.ts" -c copy a.ts
../ffmpeg -y -i "ndn:/example/data/2" -c copy result.avi
