Socket Tabanlı Opcode Client–Server Mimarisi 🎉

Lesgoooo :D !  Bu proje, client ve server arasında kendi özel protokolümüzle iletişim kurmamızı sağlıyor. Her socket geldiğinde server yeni bir thread açıyor ve progress() fonksiyonuna gönderiyor. Burada mesaj decode ediliyor, opcode’a göre işlem yapılıyor ve sonuç client’a TRAP_TABLE üzerinden dönüyor. Tadaaa :D Mutex kullandım ki dosyaya birden fazla client geldiğinde sıkıntı olmasın. 

make ile iki dosyayı da derleyebilirsiniz. 

./server  şeklinde serverı çalıştırıyoruz.

./client push elma 
şekinde push kullanabiliyoruz.
./client pop 
şeklinde de pop kullanabiliyoruz.

client, encoded olarak 3#pop şeklinde encode edip yolluyor karşı tarafa. Yani önce length sonra # sonra da length boyunca verilen mesaj. Bu tarz encode yöntemini de neetcode'da 1 sorunun çözümlerine bakarken görmüştüm ve bunu opcode ve argmanlar için kullanabileceğimi düşündüm. Bu yöntemin geliştirilebilir olduğunu düşünüyorum.


Her bağlatı accept() fonksiyonuyla socket_fd dönüyor. o socket_fd ile yeni thread oluşturuluyor ve gelen mesaj decode ediliyor. Daha sonra işlem uygulanıyor ve clientla olan bağlantı kapatılıyor.  İlk sürüm üzerine daha fazla çalışacağım 
LESGOOOOO :D
