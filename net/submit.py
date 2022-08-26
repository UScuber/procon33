import requests
import sys
import json

URL = "https://procon33-practice.kosen.work"
TOKEN = "3fc3169361f5daa766a8a3e3f757aaa20de7d4c7dafa3816ee4f7de4b9c71730"
query = "?token=" + TOKEN



"""/matchにGETリクエストを送って、試合情報のjsonを返す"""
def match_get():
    res_match = requests.get(URL + "/match" + query)
    return res_match.text


"""/problemにGETリクエストを送って、問題情報のjsonを返す"""
def problem_get():
    res_problem = requests.get(URL + "/problem" + query)
    return res_problem.text


"""/problem/chunksに分割数をPOSTリクエストで送って、各分割データのファイル名の配列のjsonを返す"""
def chunks_post():
    n = int(input("分割数を入力 "))
    res_chunks = requests.post(URL + "/problem/chunks" + query + "&n=" + str(n))
    return res_chunks.text


"""/problem/chunks/:filenameにGETリクエストを送って、分割データのwavファイルを返す"""
def file_get(res_chunks):
    # ファイル名配列のjsonを辞書型に変換する
    filenames = json.loads(res_chunks)
    # ファイル名配列の長さ分だけfor分を回し、それぞれに対応するwavファイルをダウンロードする
    for i in range(len(filenames["chunks"])):
        # ファイル名配列のi番目にあるファイルにGETリクエストを送る
        res_wav = requests.get(URL + "/problem/chunks/" + (filenames["chunks"])[i] + query)
        # wavファイルをダウンロード
        with open((filenames["chunks"])[i], "wb") as saveFile:
            saveFile.write(res_wav.content)


"""/problemに回答のjsonをPOSTリクエストで送り、返ってきたjsonファイルを受け取る"""




if __name__ == "__main__":
    match_get()
    problem_get()
    file_get(chunks_post())
