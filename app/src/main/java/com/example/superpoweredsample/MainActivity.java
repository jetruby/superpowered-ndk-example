package com.example.superpoweredsample;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.databinding.DataBindingUtil;
import android.media.AudioManager;
import android.os.Bundle;
import android.support.v4.util.Pair;
import android.support.v7.app.AppCompatActivity;
import android.widget.SeekBar;

import com.example.superpoweredsample.databinding.MainActivityBinding;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    public static final int TEMPO_HALF = 0;
    public static final int TEMPO_NORMAL = 1;
    public static final int TEMPO_DOUBLE = 2;

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("SuperpoweredExample");
    }

    boolean playing = false;
    private MainActivityBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.binding = DataBindingUtil.setContentView(this, R.layout.activity_main);
        this.binding.setPlaying(playing);

        // Get the device's sample rate and buffer size to enable low-latency Android audio output, if available.
        Pair<Integer, Integer> deviceAudioInfo = getDeviceAudioInfo();
        Integer sampleRate = deviceAudioInfo.first;
        Integer bufferSize = deviceAudioInfo.second;

        Pair<Integer, Integer> trackLocationInfo = getTrackLocationInfo();
        Integer fileOffset = trackLocationInfo.first;
        Integer fileLength = trackLocationInfo.second;

        // Arguments: path to the APK file, offset and length of the two resource files, sample rate, audio buffer size.
        SuperpoweredExample(sampleRate, bufferSize, getPackageResourcePath(), fileOffset, fileLength);

        setupViews();

    }

    private void setupViews() {
        binding.btnPlayPause.setOnClickListener((view) -> {
            setPlaying(!playing);
        });

        binding.sbPitch.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                onPitchChanged(progress - 12);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        binding.sbTrackProgress.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (progress == seekBar.getMax()) {
                    setPlaying(!playing);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                seekTo(seekBar.getProgress() / 100.0f);
            }
        });

        binding.radioGroup.check(binding.rbNormalTime.getId());
        binding.radioGroup.setOnCheckedChangeListener((group, checkedId) -> {
            switch (checkedId) {
                case R.id.rb_slow_down:
                    onTempoSelected(TEMPO_HALF);
                    break;
                case R.id.rb_normal_time:
                    onTempoSelected(TEMPO_NORMAL);
                    break;
                case R.id.rb_speed_up:
                    onTempoSelected(TEMPO_DOUBLE);
                    break;
                default:
                    onTempoSelected(TEMPO_NORMAL);
                    break;
            }
        });
    }

    private void setPlaying(boolean playing) {
        this.playing = playing;
        this.binding.setPlaying(this.playing);
        onPlayPause(this.playing);
    }

    private Pair<Integer, Integer> getDeviceAudioInfo() {
        String samplerateString;
        String buffersizeString;

        AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        samplerateString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        buffersizeString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);

        if (samplerateString == null) samplerateString = "44100";
        if (buffersizeString == null) buffersizeString = "512";

        return new Pair<>(Integer.parseInt(samplerateString), Integer.parseInt(buffersizeString));
    }

    private Pair<Integer, Integer> getTrackLocationInfo() {
        AssetFileDescriptor fd;
        Pair<Integer, Integer> trackLocationInfo = null;

        try {
            fd = getTrackAsset();
            int fileOffset = (int) fd.getStartOffset();
            int fileLength = (int) fd.getLength();
            trackLocationInfo = new Pair<>(fileOffset, fileLength);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return trackLocationInfo;
    }

    private AssetFileDescriptor getTrackAsset() throws IOException {
        return getAssets().openFd("Protest the Hero - Drumhead Trial (Instrumental).mp3");
    }

    private native void SuperpoweredExample(int samplerate, int buffersize, String apkPath,
                                            int fileOffset, int fileLength);

    private native void onPlayPause(boolean play);

    private native void seekTo(double positionPercent);

    private native void onPitchChanged(int pitchValue);

    private native void onTempoSelected(int tempoValue);

    public native String stringFromJNI();
}
