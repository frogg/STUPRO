import urllib.request, json
response = urllib.request.urlopen("http://reddit.com/.json")
content = response.read()
data = json.loads(content)