const http = require('http');
const fs = require('fs');
const path = require('path');

const uploadFile = (filePath) => {
    const fileName = path.basename(filePath);
    const stats = fs.statSync(filePath);
    const fileSizeInBytes = stats.size;
    const fileStream = fs.createReadStream(filePath);

    const options = {
        hostname: 'localhost',
        port: 3000,
        path: '/',
        method: 'GET',
        headers: {
            'Content-Type': 'video/mp4',
        }
    };

    const req = http.request(options, (res) => {
        res.setEncoding('utf8');
        res.on('data', (chunk) => {
            console.log(`BODY: ${chunk}`);
        });
        res.on('end', () => {
            console.log('No more data in response.');
        });
    });

    req.on('error', (e) => {
        console.error(`problem with request: ${e.message}`);
    });

    fileStream.pipe(req);

    fileStream.on('end', () => {
        req.end();
    });
};

// Replace 'path/to/file' with your actual file path
uploadFile('/mnt/c/Users/Exiled/Downloads/10\ hour\ timer.mp4');
