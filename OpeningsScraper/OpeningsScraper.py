from ast import parse
from time import sleep
import requests
from bs4 import BeautifulSoup
import re

cookie = {"PHPSESSID": "ajpuk5qs41gnemes4c6hlqaga3",
           "365user[user]": "hxt913",
           "365user[pass]": "b2e8629e15c26450d2e3244fcba669b3"}

def main():
    url = "https://www.365chess.com/opening.php"
    page = requests.get(url, cookies=cookie)
    if page.status_code != 200:
        print(f"Failed to retrieve page with status code: {page.status_code}")
        return
    parse_openings(page.content, [])

def parse_openings(page_content, moves):
    write = True
    soup = BeautifulSoup(page_content, 'html.parser')
    trs = soup.find_all('tr')
    for tr in trs:
        td = tr.find_all('td')
        if td[0].find('a') is None:
            continue
        move = re.sub("[0-9]+\\.\\.\\. ", "", td[0].get_text(strip=True))
        moveUrl = td[0].find('a')['href']
        try:
            num_games = int(td[1].get_text(strip=True).replace(',', ''))
        except ValueError:
            num_games = 0
        if num_games >= 1000:
            sleep(0.1)
            page = requests.get("https://www.365chess.com" + moveUrl,  cookies=cookie)
            if page.status_code == 200:
                parse_openings(page.content, moves[:] + [move])
                write = False
        else:
            if moves != [] and write:
                with open("openings.txt", "a") as f:
                    f.write(' '.join(moves) + "\n")
            break
    return

if __name__ == "__main__":
    main()