
import requests
import aiohttp
import asyncio

print("RC ROBOT CONTROL TERMINAL")

async def sendAsyncRequest(endpoint):
    async with aiohttp.ClientSession() as session:
        url = f"http://192.168.86.29:80/drive/{endpoint}"
        async with session.get(url) as resp:
            ret = await resp.json
            print(ret)

def sendSyncRequest(endpoint):
    url = f"http://192.168.86.29:80/drive/{endpoint}"
    resp = requests.get(url)
    print(resp)

while 1:
    c = input("Enter control (wasdq): ")
    if c in ['w', 'a', 's', 'd', 'q']:
        #asyncio.run(sendRequest(c))
        sendSyncRequest(c)
    else:
        print("Invalid command")
