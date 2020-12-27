import urllib3
from time import sleep
import json

http = urllib3.PoolManager(cert_reqs='CERT_NONE')
username = "Enter RollNumber"
password = "Enter Password"
URL = 'https://agnigarh.iitg.ac.in:1442/' 

while(True):
    csrf_token = str(http.request('GET', URL+'login?').data)
    st=csrf_token.find('magic" value="')+14
    en=csrf_token.find('"',st)
    csrf_token=csrf_token[st:en]
    print(csrf_token)
    data = 'username='+username+'&password='+password+'&magic='+csrf_token
    print(data)
    http.request('POST', URL,body=data)
    print("success")
    sleep(600)