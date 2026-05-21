const router = require('express').Router();
const multer = require('multer');
const auth = require('../middleware/auth');
const importController = require('../controllers/importController');

// store in memory — no need to write to disk
const upload = multer({
  storage: multer.memoryStorage(),
  limits: { fileSize: 5 * 1024 * 1024 },  // 5MB max
  fileFilter: (req, file, cb) => {
    const allowed = ['text/csv', 'application/vnd.ms-excel',
      'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'];
    if (allowed.includes(file.mimetype) || file.originalname.match(/\.(csv|xls|xlsx)$/i)) {
      cb(null, true);
    } else {
      cb(new Error('Only CSV and XLSX files are supported'));
    }
  },
});

router.post('/', auth, upload.single('file'), importController.importFile);

module.exports = router;