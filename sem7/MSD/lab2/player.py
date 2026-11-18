import sys
from pathlib import Path

from PySide6.QtCore import QSettings, Qt, QUrl
from PySide6.QtGui import QColor, QKeySequence, QPalette, QShortcut
from PySide6.QtMultimedia import QAudioOutput, QMediaPlayer
from PySide6.QtMultimediaWidgets import QVideoWidget
from PySide6.QtWidgets import (
    QApplication,
    QComboBox,
    QFileDialog,
    QHBoxLayout,
    QInputDialog,
    QLabel,
    QListWidget,
    QListWidgetItem,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QSlider,
    QSplitter,
    QStyle,
    QVBoxLayout,
    QWidget,
)

AUDIO_EXTENSIONS = ("mp3", "wav", "m4a", "flac")
VIDEO_EXTENSIONS = ("mp4", "mov", "m4v", "webm")
SKIP_MS = 10_000
VOLUME_STEP = 5
PLAYBACK_RATES = (0.5, 0.75, 1.0, 1.25, 1.5, 2.0)


def format_time(ms: int) -> str:
    total_seconds = max(0, int(ms)) // 1000
    hours, rest = divmod(total_seconds, 3600)
    minutes, seconds = divmod(rest, 60)
    if hours:
        return f"{hours}:{minutes:02d}:{seconds:02d}"
    return f"{minutes:02d}:{seconds:02d}"


def build_file_filter() -> str:
    audio = " ".join(f"*.{e}" for e in AUDIO_EXTENSIONS)
    video = " ".join(f"*.{e}" for e in VIDEO_EXTENSIONS)
    return f"Media files ({audio} {video});;Audio ({audio});;Video ({video});;All files (*)"


def apply_dark_theme(app: QApplication) -> None:
    app.setStyle("Fusion")
    palette = QPalette()
    palette.setColor(QPalette.Window, QColor(37, 37, 38))
    palette.setColor(QPalette.WindowText, QColor(230, 230, 230))
    palette.setColor(QPalette.Base, QColor(30, 30, 30))
    palette.setColor(QPalette.AlternateBase, QColor(45, 45, 48))
    palette.setColor(QPalette.Text, QColor(230, 230, 230))
    palette.setColor(QPalette.Button, QColor(55, 55, 58))
    palette.setColor(QPalette.ButtonText, QColor(230, 230, 230))
    palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
    palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
    palette.setColor(QPalette.ToolTipBase, QColor(45, 45, 48))
    palette.setColor(QPalette.ToolTipText, QColor(230, 230, 230))
    app.setPalette(palette)


class JumpSlider(QSlider):
    """Slider that jumps to the clicked position instead of stepping by a page."""

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton and self.maximum() > self.minimum():
            if self.orientation() == Qt.Horizontal:
                pos, length = event.position().x(), self.width()
            else:
                pos, length = event.position().y(), self.height()
            value = QStyle.sliderValueFromPosition(
                self.minimum(),
                self.maximum(),
                int(pos),
                length,
                self.orientation() == Qt.Vertical,
            )
            self.setValue(value)
            self.sliderMoved.emit(value)
        super().mousePressEvent(event)


class PlayerWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Media Player")
        self.resize(1000, 600)
        self.setAcceptDrops(True)

        self.settings = QSettings("LabWork", "MediaPlayerLab")
        self.current_index = -1

        self._init_player()
        self._init_ui()
        self._init_shortcuts()
        self._connect_signals()

        volume = int(self.settings.value("volume", 70))
        self.volume_slider.setValue(volume)
        self._on_volume_changed(volume)

    def _init_player(self) -> None:
        self.player = QMediaPlayer(self)
        self.audio_output = QAudioOutput(self)
        self.player.setAudioOutput(self.audio_output)
        self.video_widget = QVideoWidget()
        self.video_widget.setMinimumSize(480, 300)
        self.player.setVideoOutput(self.video_widget)

    def _init_ui(self) -> None:
        self.btn_open = QPushButton("Open...")
        self.btn_url = QPushButton("URL...")
        self.btn_prev = self._icon_button(QStyle.SP_MediaSkipBackward, "Previous track (P)")
        self.btn_back = self._icon_button(QStyle.SP_MediaSeekBackward, "Back 10 s (Left)")
        self.btn_play = self._icon_button(QStyle.SP_MediaPlay, "Play / Pause (Space)")
        self.btn_stop = self._icon_button(QStyle.SP_MediaStop, "Stop (S)")
        self.btn_forward = self._icon_button(QStyle.SP_MediaSeekForward, "Forward 10 s (Right)")
        self.btn_next = self._icon_button(QStyle.SP_MediaSkipForward, "Next track (N)")
        self.btn_mute = self._icon_button(QStyle.SP_MediaVolume, "Mute (M)")
        for btn in (self.btn_open, self.btn_url):
            btn.setFocusPolicy(Qt.NoFocus)

        self.position_slider = JumpSlider(Qt.Horizontal)
        self.position_slider.setRange(0, 0)
        self.position_slider.setFocusPolicy(Qt.NoFocus)
        self.time_label = QLabel("00:00 / 00:00")
        self.time_label.setMinimumWidth(110)
        self.time_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        self.volume_slider = QSlider(Qt.Horizontal)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setFixedWidth(120)
        self.volume_slider.setFocusPolicy(Qt.NoFocus)
        self.volume_label = QLabel("70%")
        self.volume_label.setMinimumWidth(36)

        self.speed_box = QComboBox()
        for rate in PLAYBACK_RATES:
            self.speed_box.addItem(f"{rate:g}x", rate)
        self.speed_box.setCurrentIndex(PLAYBACK_RATES.index(1.0))
        self.speed_box.setFocusPolicy(Qt.NoFocus)

        self.playlist = QListWidget()
        self.playlist.setFocusPolicy(Qt.NoFocus)
        self.playlist.setMinimumWidth(220)
        self.btn_clear = QPushButton("Clear playlist")
        self.btn_clear.setFocusPolicy(Qt.NoFocus)

        self.title_label = QLabel("Open a file, enter a URL, or drop files into the window")
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setStyleSheet("font-size: 14px; padding: 4px;")

        controls = QHBoxLayout()
        controls.addWidget(self.btn_open)
        controls.addWidget(self.btn_url)
        controls.addStretch(1)
        for btn in (self.btn_prev, self.btn_back, self.btn_play, self.btn_stop,
                    self.btn_forward, self.btn_next):
            controls.addWidget(btn)
        controls.addStretch(1)
        controls.addWidget(QLabel("Speed:"))
        controls.addWidget(self.speed_box)
        controls.addWidget(self.btn_mute)
        controls.addWidget(self.volume_slider)
        controls.addWidget(self.volume_label)

        progress = QHBoxLayout()
        progress.addWidget(self.position_slider, 1)
        progress.addWidget(self.time_label)

        left = QVBoxLayout()
        left.addWidget(self.title_label)
        left.addWidget(self.video_widget, 1)
        left.addLayout(progress)
        left.addLayout(controls)
        left_widget = QWidget()
        left_widget.setLayout(left)

        right = QVBoxLayout()
        right.addWidget(QLabel("Playlist"))
        right.addWidget(self.playlist, 1)
        right.addWidget(self.btn_clear)
        right_widget = QWidget()
        right_widget.setLayout(right)

        splitter = QSplitter(Qt.Horizontal)
        splitter.addWidget(left_widget)
        splitter.addWidget(right_widget)
        splitter.setStretchFactor(0, 4)
        splitter.setStretchFactor(1, 1)
        self.setCentralWidget(splitter)

        self.statusBar().showMessage("Ready")

    def _icon_button(self, icon: QStyle.StandardPixmap, tooltip: str) -> QPushButton:
        btn = QPushButton()
        btn.setIcon(self.style().standardIcon(icon))
        btn.setToolTip(tooltip)
        btn.setFocusPolicy(Qt.NoFocus)  # keeps Space from re-triggering the last clicked button
        return btn

    def _init_shortcuts(self) -> None:
        def add(key, slot):
            sc = QShortcut(QKeySequence(key), self)
            sc.setContext(Qt.ApplicationShortcut)
            sc.activated.connect(slot)

        add(Qt.Key_Space, self.toggle_play)
        add(Qt.Key_S, self.stop)
        add(Qt.Key_Left, lambda: self.seek_relative(-SKIP_MS))
        add(Qt.Key_Right, lambda: self.seek_relative(SKIP_MS))
        add(Qt.Key_Up, lambda: self.change_volume(VOLUME_STEP))
        add(Qt.Key_Down, lambda: self.change_volume(-VOLUME_STEP))
        add(Qt.Key_M, self.toggle_mute)
        add(Qt.Key_N, self.play_next)
        add(Qt.Key_P, self.play_previous)
        add(QKeySequence.Open, self.open_files)
        add("Ctrl+U", self.open_url)

    def _connect_signals(self) -> None:
        self.btn_open.clicked.connect(self.open_files)
        self.btn_url.clicked.connect(self.open_url)
        self.btn_play.clicked.connect(self.toggle_play)
        self.btn_stop.clicked.connect(self.stop)
        self.btn_back.clicked.connect(lambda: self.seek_relative(-SKIP_MS))
        self.btn_forward.clicked.connect(lambda: self.seek_relative(SKIP_MS))
        self.btn_next.clicked.connect(self.play_next)
        self.btn_prev.clicked.connect(self.play_previous)
        self.btn_mute.clicked.connect(self.toggle_mute)
        self.btn_clear.clicked.connect(self.clear_playlist)

        self.position_slider.sliderMoved.connect(self.player.setPosition)
        self.position_slider.sliderReleased.connect(
            lambda: self.player.setPosition(self.position_slider.value())
        )
        self.volume_slider.valueChanged.connect(self._on_volume_changed)
        self.speed_box.currentIndexChanged.connect(self._on_speed_changed)
        self.playlist.itemDoubleClicked.connect(
            lambda item: self.play_index(self.playlist.row(item))
        )

        self.player.positionChanged.connect(self._on_position_changed)
        self.player.durationChanged.connect(self._on_duration_changed)
        self.player.playbackStateChanged.connect(self._on_state_changed)
        self.player.mediaStatusChanged.connect(self._on_media_status_changed)
        self.player.errorOccurred.connect(self._on_error)
        self.audio_output.mutedChanged.connect(self._update_mute_icon)

    def add_to_playlist(self, urls: list[QUrl], autoplay: bool = True) -> None:
        if not urls:
            return
        first_new_row = self.playlist.count()
        for url in urls:
            name = Path(url.toLocalFile()).name if url.isLocalFile() else url.toString()
            item = QListWidgetItem(name)
            item.setData(Qt.UserRole, url)
            item.setToolTip(url.toString())
            self.playlist.addItem(item)
        if autoplay:
            self.play_index(first_new_row)

    def play_index(self, index: int) -> None:
        if not 0 <= index < self.playlist.count():
            return
        self.current_index = index
        self.playlist.setCurrentRow(index)
        item = self.playlist.item(index)
        url: QUrl = item.data(Qt.UserRole)
        self.player.setSource(url)
        self.player.play()
        self.title_label.setText(item.text())
        self.setWindowTitle(f"Media Player - {item.text()}")
        self.statusBar().showMessage(f"Playing: {url.toString()}")

    def open_files(self) -> None:
        start_dir = self.settings.value("last_dir", str(Path.home()))
        paths, _ = QFileDialog.getOpenFileNames(
            self, "Open media files", start_dir, build_file_filter()
        )
        if not paths:
            return
        self.settings.setValue("last_dir", str(Path(paths[0]).parent))
        self.add_to_playlist([QUrl.fromLocalFile(p) for p in paths])

    def open_url(self) -> None:
        text, ok = QInputDialog.getText(
            self, "Open URL", "Audio/video link (http/https, including .m3u8):"
        )
        if not ok or not text.strip():
            return
        url = QUrl.fromUserInput(text.strip())
        if not url.isValid() or url.scheme().lower() not in ("http", "https"):
            QMessageBox.warning(self, "Invalid URL",
                                "Enter a link starting with http:// or https://")
            return
        self.add_to_playlist([url])

    def clear_playlist(self) -> None:
        self.player.stop()
        self.player.setSource(QUrl())
        self.playlist.clear()
        self.current_index = -1
        self.title_label.setText("Playlist is empty")
        self.setWindowTitle("Media Player")

    def play_next(self) -> None:
        if self.current_index + 1 < self.playlist.count():
            self.play_index(self.current_index + 1)

    def play_previous(self) -> None:
        # restart the current track if it has been playing for a few seconds
        if self.player.position() > 3000 or self.current_index <= 0:
            self.player.setPosition(0)
        else:
            self.play_index(self.current_index - 1)

    def toggle_play(self) -> None:
        state = self.player.playbackState()
        if state == QMediaPlayer.PlayingState:
            self.player.pause()
        elif self.player.source().isEmpty():
            if self.playlist.count():
                self.play_index(max(self.current_index, 0))
            else:
                self.open_files()
        else:
            self.player.play()

    def stop(self) -> None:
        self.player.stop()

    def seek_relative(self, delta_ms: int) -> None:
        if self.player.source().isEmpty():
            return
        duration = self.player.duration()
        new_pos = self.player.position() + delta_ms
        if duration > 0:
            new_pos = min(new_pos, duration)
        self.player.setPosition(max(0, new_pos))

    def change_volume(self, delta: int) -> None:
        self.volume_slider.setValue(max(0, min(100, self.volume_slider.value() + delta)))

    def toggle_mute(self) -> None:
        self.audio_output.setMuted(not self.audio_output.isMuted())

    def _on_position_changed(self, position: int) -> None:
        # don't fight the user while the slider is being dragged
        if not self.position_slider.isSliderDown():
            self.position_slider.setValue(position)
        self._update_time_label(position)

    def _on_duration_changed(self, duration: int) -> None:
        self.position_slider.setRange(0, max(0, duration))
        self._update_time_label(self.player.position())

    def _update_time_label(self, position: int) -> None:
        self.time_label.setText(
            f"{format_time(position)} / {format_time(self.player.duration())}"
        )

    def _on_state_changed(self, state) -> None:
        icon = (QStyle.SP_MediaPause if state == QMediaPlayer.PlayingState
                else QStyle.SP_MediaPlay)
        self.btn_play.setIcon(self.style().standardIcon(icon))
        if state == QMediaPlayer.StoppedState:
            self.position_slider.setValue(0)
            self._update_time_label(0)

    def _on_media_status_changed(self, status) -> None:
        if status == QMediaPlayer.EndOfMedia:
            if self.current_index + 1 < self.playlist.count():
                self.play_next()
            else:
                self.statusBar().showMessage("Playback finished")

    def _on_volume_changed(self, value: int) -> None:
        self.audio_output.setVolume(value / 100)
        self.volume_label.setText(f"{value}%")
        self.settings.setValue("volume", value)
        if value > 0 and self.audio_output.isMuted():
            self.audio_output.setMuted(False)

    def _on_speed_changed(self, _index: int) -> None:
        self.player.setPlaybackRate(float(self.speed_box.currentData()))

    def _update_mute_icon(self, muted: bool) -> None:
        icon = QStyle.SP_MediaVolumeMuted if muted else QStyle.SP_MediaVolume
        self.btn_mute.setIcon(self.style().standardIcon(icon))

    def _on_error(self, error, message: str) -> None:
        if error == QMediaPlayer.NoError:
            return
        source = self.player.source().toString() or "-"
        self.statusBar().showMessage(f"Error: {message}")
        QMessageBox.warning(
            self,
            "Playback failed",
            f"{message}\n\nSource: {source}\n\n"
            "Check that the file exists, its format is supported by the system "
            "(mp3, wav, m4a, flac, mp4, mov, m4v, webm), and the link is reachable.",
        )

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()

    def dropEvent(self, event):
        urls = [u for u in event.mimeData().urls() if u.isLocalFile()]
        supported = AUDIO_EXTENSIONS + VIDEO_EXTENSIONS
        urls = [u for u in urls if Path(u.toLocalFile()).suffix.lower().lstrip(".") in supported]
        if urls:
            self.add_to_playlist(urls)
        else:
            self.statusBar().showMessage("Unsupported file format")


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("Media Player")
    apply_dark_theme(app)
    window = PlayerWindow()
    window.show()
    return app.exec()


if __name__ == "__main__":
    sys.exit(main())