import requests
import sys
import json
import os
import urllib3
from urllib3.exceptions import InsecureRequestWarning
urllib3.disable_warnings(InsecureRequestWarning)


URL = "https://procon33-practice.kosen.work"
TOKEN = "3fc3169361f5daa766a8a3e3f757aaa20de7d4c7dafa3816ee4f7de4b9c71730"
query = "?token=" + TOKEN


"""エラー一覧"""
err = ["InvalidToken", "AccessTimeError", "FormatError", "NotFound", "TooLargeRequestError", "Gateway Timeout"]


"""予期せぬ結果が出たときに例外処理する関数"""
def alert(j):
    if j is None:
        print("None!!")
        sys.exit()
    for i in range(len(err)):
        if err[i] in j.text:
            print(j.text)
            sys.exit()


"""/matchにGETリクエストを送って、試合情報のjsonを返す"""
def match_get():
    res_match = requests.get(URL + "/match" + query, verify=False)
    alert(res_match)
    print(res_match.text)
    return res_match.text


"""/problemにGETリクエストを送って、問題情報のjsonを返す"""
def problem_get():
    res_problem = requests.get(URL + "/problem" + query, verify=False)
    alert(res_problem)
    print(res_problem.text)
    # 受けとったjsonを辞書型に変換
    problem_dic = json.loads(res_problem.text)
    # 変換したjsonから選択数を抽出する
    return int(problem_dic["chunks"])


"""/problem/chunksに分割数をPOSTリクエストで送って、各分割データのファイル名の配列のjsonを返す"""
def chunks_post(select):
    res_chunks = requests.post(URL + "/problem/chunks" + query + "&n=" + str(select), verify=False)
    alert(res_chunks)
    print(res_chunks.text)
    return res_chunks.text


"""/problem/chunks/:filenameにGETリクエストを送って、分割データのwavファイルを返す"""
def file_get(res_chunks):
    # ファイル名配列のjsonを辞書型に変換する
    filenames = json.loads(res_chunks)
    # audioディレクトリに移動
    os.chdir("audio")
    # ファイル名配列の長さ分だけfor分を回し、それぞれに対応するwavファイルをダウンロードする
    for i in range(len(filenames["chunks"])):
        # ファイル名配列のi番目にあるファイルにGETリクエストを送る
        res_wav = requests.get(URL + "/problem/chunks/" + (filenames["chunks"])[i] + query, verify=False)
        # audioディレクトリにwavファイルをダウンロード
        with open((filenames["chunks"])[i], "wb") as saveFile:
            saveFile.write(res_wav.content)
    # ディレクトリを元に戻す
    os.chdir("..")


"""/problemに回答のjsonをPOSTリクエストで送り、返ってきたjsonファイルを受け取る"""
def problem_post():
    res_problem = requests.post(URL + "/problem" + query, json.dump({}), headers={'Content-Type': 'application/json'}, verify=False)



if __name__ == "__main__":
    match_get()
    select = problem_get()
    file_get(chunks_post(select))
