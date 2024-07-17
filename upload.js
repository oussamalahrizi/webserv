const http = require('http');
const fs = require('fs');

const uploadFile = (filePath) => {
    const stats = fs.statSync(filePath);
    const fileSizeInBytes = stats.size;
    const fileStream = fs.createReadStream(filePath);

    const options = {
        hostname: 'localhost',
        port: 8080,
        path: '/deny',
        method: 'POST',
        headers: {
            'Content-Type': 'video/mp4',
            'Content-Length': fileSizeInBytes
        }
    };
    const req = http.request(options);

    req.on('response', (res) => {
        res.setEncoding('utf8');
        res.on('data', (chunk) => {
            console.log(`BODY: ${chunk}`);
        });
        res.on('end', () => {
            console.log('No more data in response.');
        });
    });

    req.on('close', () => {
        console.log('Request closed');
        fileStream.destroy(); // Ensure file stream is closed when request ends
    });

    fileStream.on('error', (err) => {
        console.error(`File stream error: ${err.message}`);
        req.destroy(err); // Destroy the request if there's a file stream error
    });

    fileStream.pipe(req);

    fileStream.on('end', () => {
        console.log('File stream ended');
    });
};

// Replace 'path/to/file' with your actual file path
uploadFile('/nfs/homes/olahrizi/Downloads/video.mp4');