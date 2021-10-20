# Dropbox
Basic Dropbox

ServerY
Çalıştırılması için gerekenler:
Klasörde makefile dosyası olduğu için make komutu ile program derlenip çalıştırılabilir hale getirilebilir. ServerY’ı çalıştırmak için girilecek argümanlar ;
- clientları dinleyen bağlantı portu
- ServerZ bağlantısı için ip adresi
- ServerZ dinleyen bağlantı portu
- PoolPro da çalışacak thread sayısı
- PoolForward çalışacak thread sayısı
- işlemi tamamladıktan sonra uyku süresi
- log dosyası ismi
Örnek: ./ServerYExe [portNumber] [IpAddressZ] [portNumber] [PoolProSize] [PoolForwardSize] [time] [LogFile]


ServerY tasarımı :
Main ilk olarak clientlara bağlanmak için socket oluşturur. Çıkma komutu gelmediği sürece sonsuza kadar dönen while döngüsü içerisinde bağlanan clientların accept den dönen değeri alınır. While döngüsü içerisinde ilk olarak PoolForward daki thread sayısını tutan değişken tarafından kontrol edilir. Eğer PoolForward sınırını geçtiyse yönlendirelecek bir yer kalmamıştır, “NO THREAD IS AVAILABLE” loglaması yapılır. PoolForward sınırı aşılmadıysa koşuldan içeriye girilir ve ikinci bir koşul olan PoolProSize kontrol edilir. Bu kontrol ise gelen thread in nereye yönlendireleceğini belirlemek içindir. PoolProSize sınırını geçmediyse thread create edilerek accept den dönen değer ile thread PoolPro için çalışan metoduna yönlendirilir ve o havuza özel sayaç artırılır, PoolProSize sınırını geçdiyse thread create edilerek accept den dönen değer ile thread PoolForward için çalışan metoduna yönlendirilir ve o havuza özel sayaç artırılır.
PoolPro için yönlendirilen “conThreadFunc” metodunda ilk olarak bağlanılan client ın ismi alınır ve threadın çalışıp çalışamıycak durumda olduğu bilgisi bir değişkenle client a gönderilir. Bu durumun asıl amacı PoolForward lar için gerekli olmasıdır. Client gelen thread statüsüne göre ya çalışacaktır yada error message ını basacaktır. Client, olumlu statü mesajını aldıktan sonra random olarak oluşturduğu 16x16 lık matrisi ServerY daki PoolPro threadine yaptıracaktır. PoolPro da çalışan thread gelen matrisi aldıktan sonra, sözde Fourier transform yapan (zaman konusunda sıkıntı yaşadığım için fourier için çalışan fonksiyondan gelen matris içindeki değerlerin hepsi ile bağlantılı şekilde oynanarak yeni bir matris üretilmiştir.) metoda gönderilir ve dönüşüm yapıldıktan sonra komut olarak girilen bekleme süresi uygulanılarak tekrar client a gönderilir. Client matrisi aldıktan sonra işlem süresi hesaplamasını yaparak tekrardan ServerY ye geri gönderir. Poolpro metodu en sonunda poolpro için çalışan thread sayacını mutex içinde azaltır ve log işlemini yaparak görevini sonlandırır.
PoolForward için yönlendirilen “ForwarrdThreadFunc” metodunda ilk olarak bağlanılan client ın ismi alınır. ServerZ için socket bağlantısı yapılır. Burada ServerZ nin ServerY den önce kapanma durumu kontrol edilir. Bu kontrolden sonra ise ServerZ den kontrol için statü değişkeni beklenir .ServerZ deki threadler müsaitse ve threadın çalışıp çalışamıycak durumda olduğu bilgisi bir değişkenle client a gönderilir. Client gelen thread statüsüne göre ya çalışacaktır yada error message ını basacaktır. Client, olumlu statü mesajını aldıktan sonra random olarak oluşturduğu 16x16 lık
matrisi ServerY daki PoolForward threadine gönderecektir, poolForward ise işlemleri yapmak üzere gelen matrisi ServerZ ye gönderecektir. ServerZ deki PoolPro da çalışan thread gelen matrisi aldıktan sonra, sözde Fourier transform yapan metoda gönderilir ve dönüşüm yapıldıktan sonra komut olarak girilen bekleme süresi uygulanılarak tekrar ServerY deki poolForward threadine gönderilir. PoolForwardaki thread ise aldığı matrisi client a gönderir. Client matrisi aldıktan sonra işlem süresi hesaplamasını yaparak tekrardan ServerY yedeki poolForward threadlerine geri gönderir. PoolForward metodu en sonunda PoolForward için çalışan thread sayacını mutex içinde azaltır ve log işlemini yaparak görevini sonlandırır.


ServerZ
Çalıştırılması için gerekenler:
Klasörde makefile dosyası olduğu için make komutu ile program derlenip çalıştırılabilir hale getirilebilir. ServerZ’ı çalıştırmak için girilecek argümanlar;
- clientları dinleyen bağlantı portu
- PoolPro da çalışacak thread sayısı
- işlemi tamamladıktan sonra uyku süresi
- log dosyası ismi
Örnek: ./ServerZExe [portNumber] [PoolProSize] [time] [LogFile]

ServerZ tasarımı :
Main ilk olarak ServerY deki threadlere bağlanmak için socket oluşturur. Çıkma komutu gelmediği sürece sonsuza kadar dönen while döngüsü içerisinde bağlanan clientların accept den dönen değeri alınır. While döngüsü içerisinde ilk olarak PoolProSize kontrol edilir. PoolProSize sınırını geçmediyse thread create edilerek accept den dönen değer ile thread PoolPro için çalışan metoduna yönlendirilir ve o havuza özel sayaç artırılır.
PoolPro için yönlendirilen “conThreadFunc” metodunda ilk olarak bağlanılır ve ServerZ deki threadın çalışıp çalışamıycak durumda olduğu bilgisi bir değişkenle ServerY ye gönderilir. ServerY gelen thread statüsüne göre ya çalışacaktır yada error message ını basacaktır. Çalışmaya devam ettiği durumda PoolPro da çalışan thread gelen matrisi aldıktan sonra, sözde Fourier transform yapan metoda gönderir ve dönüşüm yapıldıktan sonra komut olarak girilen bekleme süresi uygulanılarak tekrar ServerY ye gönderilir. ServerY matrisi aldıktan sonra işlem süresi hesaplamasını yaparak tekrardan ServerY ye geri gönderir. Poolpro metodu en sonunda poolpro için çalışan thread sayacını mutex içinde azaltır ve log işlemini yaparak görevini sonlandırır.


Client
Çalıştırılması için gerekenler:
Klasörde makefile dosyası olduğu için make komutu ile program derlenip çalıştırılabilir hale getirilebilir. Client’ı çalıştırmak için girilecek argümanlar ;
- clientları ismi
- ServerY bağlantısı için ip adresi
- ServerY bağlantı portu
Örnek: ./ClientExe [ClientName] [IpAddressY] [portNumberY]

Client tasarımı :
Main ilk olarak ServerY deki threadlere bağlanmak için socket oluşturur. ServerY ye bağlandıktan sonra ismini gönderir. Sonraındaki aşamada ServerY den gelen statü durumuna göre hareket eder. Eğer statü 0 ise error mesajı basarak işemini tamamlar , eğer statü 1 ise random array üreterek Server Y ye gönderir. Server ye işemleri yaptıktan sonra tekrar clienta yeni matrisi gönderir. Client bu matrisi aldıktan sonra geçen işlem süresini hesaplar ve ServerY ye bilgi amaçlı gönderir. Gönderme işlemi de tamamlandıktan sonra client süreci tamamlanır.


Notlar:
Ctrl-C için sinyal handler edilmiştir.
Loglama işlemi yapılmaktadır.
Test için Script oluşturulmuştur.

Test Edilmesi
İlk olarak klasör içindeki -make komutu çalıştırılır. Bu komut dan sonra script i çalıştırmak için
- chmod +x Dropbox.sh
- ./Dropbox.sh
- 
Komutları çalıştırılır. Bu son komutdan sonra 5 test aşamasından oluşan süreç başlar. İlk teste başlamak için herhangi bir değere basılır.

Test1 de : ServerY de 2 PoolPro ve 2 PoolForward ve 4 saniye, ServerZ de 2 PoolPro ve 3 saniye argümanları ile 3 client başlatılmış ve doğru sonuçlar alınmıştır.
Test2 de : ServerY de 2 PoolPro ve 2 PoolForward ve 3 saniye, ServerZ de 2 PoolPro ve 2 saniye argümanları ile 5 client başlatılmış ve doğru sonuçlar alınmıştır. 5. Gelen client yer bulamadığı için error vermiştir.
