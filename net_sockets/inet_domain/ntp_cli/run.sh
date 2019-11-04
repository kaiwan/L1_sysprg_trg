# run.sh
# See http://tf.nist.gov/tf-cgi/servers.cgi
# for NTP servers.
# ...
# 3. The generic name time.nist.gov will continue to point to all of our 
#    servers on a round-robin basis, and users are encouraged to access the service using this name.
# ...
./ntp_cli time.nist.gov 123

# See http://stackoverflow.com/questions/10757575/how-to-write-a-ntp-client to see howto
# talk to the NTP server..
