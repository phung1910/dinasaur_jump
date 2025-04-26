Vào năm 2120, khủng long đã được tái tạo lại và trở thành vật nuôi của con người. Trong lúc chơi đùa cùng khủng long, Andy đã bị lạc vào 1 khu rừng. Game theo chân Andy và chú khủng long vượt qua các chướng ngại vật để tìm đường về nhà. 

How to play:

Trong trò chơi DINOJUMP, nhấn space để nhảy khi gặp chướng ngại vật (Lưu ý: Nên nhảy khi đến gần chướng ngại vật). Ở trò chơi 2 - FLAPPY DINO , nhấn space liên tục để k bị rơi xuống, khi khủng long chạm đất sẽ thua. 

Sau khi thành công vượt qua 5 chướng ngại vật ở DINAJUMP, khủng long sẽ phải trải qua ma trận đá ở trò chơi thứ 2, sau khi vượt qua thêm 5 chướng ngại vật ở ma trận đá, trò chơi sẽ kết thúc.

CẤU TRÚC CHÍNH CỦA GAME:

1.Nền của game DINOJUMP bắt đầu từ ground level, khủng long và chướng ngại vật sẽ có điểm bắt đầu bằng ground level.

2.Khủng long sẽ có các chỉ số khởi đầu là chiều dài, chiều rộng, trọng lượng và độ cao cú nhảy, các chướng ngại vật sẽ có chiều cao và chiều rộng thay đổi ngẫu nhiên.

3.Hoạt ảnh khủng long di chuyển được tạo nên từ 3 hình ảnh. Các làm hoạt ảnh và điều khiển bằng phím được tham khảo từ video https://www.youtube.com/watch?v=BZaTNiJAHE4 . 

4.Logic va chạm của game: Game DINOJUMP sử dụng kỹ thuật AABB (Axis-Aligned Bounding Box), tức là kiểm tra xem hai hình chữ nhật bao quanh nhân vật và chướng ngại vật có giao nhau hay không. Cụ thể trục X: dinoRight > obsLeft && dinoLeft < obsRight (các hình chữ nhật chồng lấn theo chiều ngang). trục Y: dinoBottom > obsTop && dinoTop < obsBottom (các hình chữ nhật chồng lấn theo chiều dọc).

Game FLAPPY DINA sử dụng logic tương tự, sự khác biệt là thay vì tránh va chạm khi tiếp xúc chướng ngại vật thì khủng long phải đi vào giữa 2 cục đá.

Cơ chế va chạm được tham khảo từ video https://www.youtube.com/watch?v=c_6qEdSAw8Y .

5.Cơ chế vật phẩm tăng giảm kích thước của khủng long, xuất hiện 1 lần sau khi obstacles = 3, tỷ lệ xuất hiện 1 trong 2 vật phẩm to nhỏ là 50%. Các kích thước chướng ngại vật giữ nguyên => nếu may mắn ăn được cục vật phẩm thu nhỏ trò chơi sẽ dễ dàng hơn và ngược lại.

UPDATE:

19/3: Tạo code nền, có thể chạy với hình ảnh đơn giản, chỉ có thể nhảy và thua khi va chạm.

25/3: Tạo thêm tính năng play again, chỉnh lại giao diện.

3/4: Update thêm tính năng ẩn, map mới.
     Thêm tính năng đếm số lượt chướng ngại vật đã qua.
     Khi vượt qua 5 chướng ngại vật, người chơi sẽ được chuyển qua map mới, khi vượt qua 5 chướng ngại vật ở map mới, người chơi sẽ chiến thắng.

7/4: Update đồ họa khủng long, chướng ngại vât, nền.
     Update hình ảnh khi thua, replay trò chơi, hình ảnh khi chiến thắng.

13/4 Upload bản 1.0 của game: khủng long vượt qua chướng ngại vật. Tuy nhiên còn xảy ra lỗi chướng ngại vật k vượt qua được, hình ảnh còn chưa được chi tiết.

25/4 Update bản 2.0 của game: tạo thêm cơ chế vật phẩm làm cho tựa game có thêm yếu tố may rủi. Update hình ảnh và cơ chế game sao cho cân bằng độ khó và cải thiện hitbox.
