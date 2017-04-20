from glob import glob 
import requests
from time import time
import requests_unixsocket

requests_unixsocket.monkeypatch()

for file in glob('*.html'):
	with open(file) as f:
		html = f.read()
		b = int(time() * 1000)
		# r = requests.post('http+unix://%2Fvar%2Frun%2Ftest.sock?document=1&context=0&html=1&element=0&attribute=0', data=html)
		r = requests.post('http://127.0.0.1:8888?document=1&context=0&html=1&element=0&attribute=0', data=html)
		text = r.text
		e = int(time() * 1000)
		print('%s %sKB to %sKB with %dms' % (file, len(html)/1024, len(text)/1024, e - b))
		

