public class p4Activity extends Activity {
    private static final String TAG = "p4Activity";
    private static final boolean D = true;

    // Request constants
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    // Bluetooth related
    private String mConnectedDeviceName = null;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BTService mBTService = null;

    // UI Widgets
    private Button mBtBTN;
    private Resources mAppRes;
    private ProgressDialog mProgress;

    // Music thread
    private MusicThread musicThread = null;
    private boolean isPlaying = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_p4);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        if (D) Log.e(TAG, "++ ON START ++");

        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        } else {
            if (mBTService == null) initUI();
        }
    }

    private void initUI() {
        mAppRes = this.getResources();
        mBTService = new BTService(this, mHandler);
    }

    // Music notes commands
    private static final String[] NOTES = {
        "@BUZ,a#", // 도
        "@BUZ,s#", // 레
        "@BUZ,d#", // 미
        "@BUZ,f#", // 파
        "@BUZ,g#", // 솔
        "@BUZ,h#", // 라
        "@BUZ,j#"  // 시
    };

    // Song patterns
    private static final int[] RABBIT_SONG = {0,0,2,2,4,4,2, 2,2,0,2,4,2, 0,0,2,2,4,4,2, 2,0,2,0,-1};
    private static final int[] STAR_SONG = {0,0,4,4,5,5,4, 3,3,2,2,1,1,0, 4,4,3,3,2,2,1, 4,4,3,3,2,2,1, 0,0,4,4,5,5,4, 3,3,2,2,1,1,0,-1};

    public void clickFunc(View v) {
        if (mBTService.getState() != BTService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        switch(v.getId()) {
            case R.id.btnDo:
                sendMessage(NOTES[0]);
                break;
            case R.id.btnRe:
                sendMessage(NOTES[1]);
                break;
            case R.id.btnMi:
                sendMessage(NOTES[2]);
                break;
            case R.id.btnFa:
                sendMessage(NOTES[3]);
                break;
            case R.id.btnSol:
                sendMessage(NOTES[4]);
                break;
            case R.id.btnLa:
                sendMessage(NOTES[5]);
                break;
            case R.id.btnSi:
                sendMessage(NOTES[6]);
                break;
            case R.id.btnRabbit:
                startSong(RABBIT_SONG);
                break;
            case R.id.btnStar:
                startSong(STAR_SONG);
                break;
            case R.id.btnStop:
                stopSong();
                break;
        }
    }

    private void startSong(final int[] notes) {
        stopSong();
        isPlaying = true;
        musicThread = new MusicThread(notes);
        musicThread.start();
    }

    private void stopSong() {
        isPlaying = false;
        if (musicThread != null) {
            musicThread.interrupt();
            musicThread = null;
        }
        sendMessage("@BUZ,k#"); // Stop sound
    }

    private class MusicThread extends Thread {
        private final int[] notes;

        public MusicThread(int[] notes) {
            this.notes = notes;
        }

        @Override
        public void run() {
            try {
                for (int note : notes) {
                    if (!isPlaying) break;
                    if (note == -1) break;

                    sendMessage(NOTES[note]);
                    Thread.sleep(500); // Note duration
                }
            } catch (InterruptedException e) {
                // Thread interrupted, stop playing
            } finally {
                sendMessage("@BUZ,k#"); // Stop sound
            }
        }
    }

    private void sendMessage(String message) {
        if (mBTService.getState() != BTService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }
        if (message.length() > 0) {
            byte[] send = message.getBytes();
            mBTService.write(send);
        }
    }

    private void connectDevice(Intent data) {
        String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        mBTService.connect(device);
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Constant.MESSAGE_STATE_CHANGE:
                    if(D) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                    switch (msg.arg1) {
                        case BTService.STATE_CONNECTED:
                            if(mProgress != null) mProgress.dismiss();
                            break;
                        case BTService.STATE_CONNECTING:
                            showProgressDialog("Bluetooth");
                            break;
                        case BTService.STATE_LISTEN:
                        case BTService.STATE_NONE:
                            break;
                    }
                    break;
                case Constant.MESSAGE_DEVICE_NAME:
                    mConnectedDeviceName = msg.getData().getString(Constant.DEVICE_NAME);
                    Toast.makeText(getApplicationContext(), "Connected to " + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                    break;
                case Constant.MESSAGE_TOAST:
                    if(msg.getData().getInt(Constant.TOAST) == R.string.unable_connected)
                        if(mProgress != null) mProgress.dismiss();
                    Toast.makeText(getApplicationContext(), msg.getData().getInt(Constant.TOAST), Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    private void showProgressDialog(String szType) {
        Log.i(TAG,"showProgressDialog()");
        mProgress = new ProgressDialog(this, R.style.ProgressDialog);
        mProgress.setCancelable(false);
        mProgress.setMessage(" [ " + szType + " ] " + mAppRes.getString(R.string.title_connecting));
        mProgress.show();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mBTService != null) mBTService.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
        if(mProgress != null) mProgress.dismiss();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(D) Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE:
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data);
                }
                break;
            case REQUEST_ENABLE_BT:
                if (resultCode == Activity.RESULT_OK) {
                    initUI();
                } else {
                    Log.d(TAG, "BT not enabled");
                    Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                    finish();
                }
                break;
        }
    }
}