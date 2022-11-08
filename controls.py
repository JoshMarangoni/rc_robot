
import requests
import aiohttp
import asyncio

print("RC ROBOT CONTROL TERMINAL")

async def sendRequest(endpoint):
    async with aiohttp.ClientSession() as session:
        url = f"blah blah blah{endpoint}"
        async with session.get(url) as resp:
            ret = await resp.json
            print(ret)

while 1:
    c = input("Enter control (wasd): ")
    if c in ['w', 'a', 's', 'd']:
        asyncio.run(sendRequest(c))
    else:
        print("Invalid command")
