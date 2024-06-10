document.addEventListener("DOMContentLoaded", function() {
    const video = document.getElementById('video');
    const canvas = document.getElementById('canvas');
    const snap = document.getElementById('snap');
    const camera_image = document.getElementById('camera_image');

    if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
        navigator.mediaDevices.getUserMedia({ video: true }).then(function(stream) {
            video.srcObject = stream;
            video.play();
        }).catch(function(error) {
            console.error("Something went wrong!", error);
        });
    }

    snap.addEventListener('click', function() {
        canvas.style.display = 'block';
        const context = canvas.getContext('2d');
        context.drawImage(video, 0, 0, 320, 240);
        camera_image.value = canvas.toDataURL('image/jpeg');
    });
});
