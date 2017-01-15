#include<stddef.h>

#define NAME_CONFLICT 1
#define NO_SPACE 2
#define NOT_FOUND 3


/* probably useful websites
http://www.geocities.jp/sugachan1973/doc/funto31.html  from middleish
http://www.ie.u-ryukyu.ac.jp/~kono/os/ex/file/file.html
http://kukuruku.co/hub/nix/writing-a-file-system-in-linux-kernel
http://stackoverflow.com/questions/29218231/implementing-my-own-read-write-open-close-functions-for-an-ext2-file-system
https://lwn.net/Articles/57369/
https://github.com/psankar/simplefs
https://github.com/masami256/miko
https://github.com/Xilinx/linux-xlnx/blob/master/fs/fat/namei_msdos.c

http://www.cc.kyoto-su.ac.jp/~yamada/pB/struct.html
http://www.usupi.org/sysad/242.html



/* Parameters:
 * 	char*, filename (must be null terminated)
 * 	void*, data to save in file
 * 	size_t, amount of data to save
 * When invoked, this function will read the amount of data indicated by the third
 * parameter from the memory area indicated by the second, and save the data on
 * the hard drive if there is room.  If room exists and no file with the same name
 * exists, and the save is successful, the command should return 0.  If there is
 * another file with the same name, the return value is NAME_CONFLICT.  If there is
 * no more space on the hard drive, the return value is NO_SPACE. */
int save(char*, void*, size_t);
/*感想、アイディア
QuizPass LINKS
１番目はファイル名、２番目はcharのデータそのもの、３番目はデータサイズ。
データサイズが一番簡単で、２番目はread_sectorの３番目のところと一致するところがある。←後日誤りと断定
ということは、read_sectorの１番目（シリンダー）と２番目セクター番号が重ならないよう確認をしたのち
適切なところにデータをセーブさせる。
なお０は成功、その他の数字はすべてエラーと見なされるため、基本的にはret=saveなんちゃらの場合、０を出させたい。
以上が第一目標。

１０月６日記入
以下
saveとloadの共通点はタイトルのみ。
ということは、read_sectorで同ファイル確認ができるハズ？
loadの２番目はただの入れる場所。
構造体を使用して、データ管理をしやすくする主に　bigfile[i]をイジるために
http://www.tuat.ac.jp/~yamada/cp1/cp1-8.html
ともっと構造体に詳しいサイトを参考とせよ

１０月８日記入
format は overwrite everything with a[0]。
write_sectorは１０２４バイトしかコピーしないので気をつけたし
loadの２番目は入れる場所であり、比較するための仮の場所である。
よってloadされた内容があっているかどうかはstrcmpで行われる。
load３番目は２番目の配列のサイズ。
－　ネームチェック方法がわからない。
write_sectorは何かが書いてあるchar配列、read_sectorは空いているchar配列。
https://www.grapecity.com/tools/support/powernews/column/clang/029/page04.htm
ここに配列の理想的な分割方法があるかも、より深く確認されたし
もしやファイル名をwrite_sectorの最初のほうに挿入するワンチャン？
つまり、write_sectorの入力するchar配列の最初のほうにタイトルをねじ込む。
あるいは入力用のchar配列の最初をタイトルにして、その後に本来セーブするデータを入れ込む。
http://f4.aaacafe.ne.jp/~pointc/log825.html
https://www.grapecity.com/tools/support/powernews/column/clang/029/page04.htm

http://aima.cs.berkeley.edu/2nd-ed/newchap07.pdf
↑logic agent

loadの際に、ファイル名数分のiをすっ飛ばしたコピーarrayを出させる？

１０月１１日記入
http://stackoverflow.com/questions/17542541/simple-strcat-implementation-with-pointers
http://codereview.stackexchange.com/questions/40616/pointer-version-of-strcat
http://stackoverflow.com/questions/14259540/c-programming-strcat-using-pointer
http://spitzayano.web.fc2.com/arrangement/c/pointer.html
http://sealsoft.jp/ptr_and_ref.html

Test３をかんがみるに、saveとloadでは内容の変更が生じる可能性あり。

Test4を鑑みるに、マルチセクターなるものをやらねばならない。
調査必須。
調査１０分の結果、わからず。仲間に聞かれたし。
４番目は、実行を行わずにただエラーを吐き返せば良い？実際にセーブする必要はなし？sethに聞かれたし。


fdelete = overwrite if there is somthing
and say its avaiable if there is nothing.

Test6,7はエラー対処方法をsaveとloadに導入するのみ。
両方のパラメーターを鑑みるに、全サーチをする必要は無い模様。

Test8は同名ファイルのセーブを受け付けないことを証明する。
どうやら内容は一致する必要は無い模様。
つまり名前をベースに初期検索を行う必要がある。
function内にcharを用意し、そこから内容の不一致を確認する。

Test9-10は思いつかない。
Test9内容の説明
ブルートフォースを通して空きスペースが無い場合のエラーを吐き出せばいいのだろうか？←それでいいらしい。
どうやら入力を最優先し、どんな既存ファイルも上書きをする予定。
saveをやる回数自体は多いので、既に同じシリンダーセクター内にファイルがあったとしても上書きすればいい模様。

multi-sector thingy
google how does the FAT work,
もしデータ必要数が多かった場合、それらを分割し、分割された分空いているセクタを検索。

Test10
誤解はあったが、どうやらsaveの回数を繰り返すたびにbigtの後ろに数字がつけられる模様。
これがまったくわからない。

Test11
ファイルが見つからない場合NOT_FOUNDを返す。
どうやらプログラムミスで、消す回数が一致しない場合もあり。

Test12
再フォーマット。ガス曰く簡単な模様。
*/

/* Parameters:
 * 	char*, filename (must be null terminated)
 * 	void*, location to store data from file
 * 	size_t, size of the area indicated by the second parameter
 * When invoked, this function will read the indicated file form the hard drive,
 * and copy the data from the file into the area indicated by the second parameter.
 * The third parameter indicates the maximum amount which can be written to the
 * designated memory area without causing a buffer overflow, and this function should
 * not write more than this amount.  If the space is inadequate, as much of the file
 * as can be read should be read, and the return value will be NO_SPACE.  If the
 * file does not exist, the return value will be NOT_FOUND.  If the load was successful,
 * and the entire file fit into the allocated space, the return value should be 0.  */
int load(char*, void*, size_t);

/* Parameters:
 * 	char*, filename (must be null terminated)
 * When invoked, this function will remove the indicated file.  If the file does
 * not exist, the return value is NOT_FOUND.  Otherwise, the return value should
 * be 0. */
int fdelete(char*);

/* Formats the filesystem.  This should set up an initial file allocation table and
 * free space table, and anything else you need.  It will be called before any of
 * the other functions in this file.  It will also remove all files on the
 * filesystem if called later. */
void format();

