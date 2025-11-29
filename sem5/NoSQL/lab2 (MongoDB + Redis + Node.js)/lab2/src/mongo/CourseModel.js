const mongoose = require('mongoose');

// MongoDB schema; it's not plain as SQL one, it can be nested
const LessonSchema = new mongoose.Schema({
    sql_lesson_id: Number,
    title: String,
    duration: Number
});

const SectionSchema = new mongoose.Schema({
    sql_section_id: Number,
    title: String,
    order: Number,
    lessons: [LessonSchema] // array of lessons inside the section
});

const CourseSchema = new mongoose.Schema({
    sql_course_id: { type: Number, index: true }, 
    title: String,
    sections: [SectionSchema], // array of sections inside the course
    synced_at: { type: Date, default: Date.now }
});

module.exports = mongoose.model('MongoCourse', CourseSchema);