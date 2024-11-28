public class P5Activity extends Activity {
    private static final String TAG = "P5Activity";
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    // BT related
    private BluetoothAdapter mBluetoothAdapter = null;
    private BTService mBTService = null;
    private String mConnectedDeviceName = null;

    // UI Components
    private Button mBtConnectBtn, mCheckSpeedBtn, mLeftBtn, mUpBtn,
                   mRightBtn, mDownBtn, mStopBtn, mDecreaseBtn, mIncreaseBtn;
    private TextView mSpeedTextView;
    private ProgressDialog mProgress;

    // State variables
    private int currentSpeed = 50;
    private int currentAngle = 90; // Center position for servo
    private boolean isRotating = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_p5);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        initUI();
    }

    private void initUI() {
        mBtConnectBtn = findViewById(R.id.btConnectBtn);
        mCheckSpeedBtn = findViewById(R.id.checkSpeedBtn);
        mLeftBtn = findViewById(R.id.leftBtn);
        mUpBtn = findViewById(R.id.upBtn);
        mRightBtn = findViewById(R.id.rightBtn);
        mDownBtn = findViewById(R.id.downBtn);
        mStopBtn = findViewById(R.id.stopBtn);
        mDecreaseBtn = findViewById(R.id.decreaseBtn);
        mIncreaseBtn = findViewById(R.id.increaseBtn);
        mSpeedTextView = findViewById(R.id.speedTextView);

        mBtConnectBtn.setOnClickListener(v -> {
            if (mBTService != null) mBTService.stop();
            startActivityForResult(new Intent(this, DeviceListActivity.class), REQUEST_CONNECT_DEVICE);
        });

        mCheckSpeedBtn.setOnClickListener(v -> {
            updateSpeedDisplay();
        });

        mLeftBtn.setOnClickListener(v -> {
            currentAngle = Math.max(0, currentAngle - 1);
            sendMessage("@SRV," + currentAngle + "#");
        });

        mRightBtn.setOnClickListener(v -> {
            currentAngle = Math.min(180, currentAngle + 1);
            sendMessage("@SRV," + currentAngle + "#");
        });

        mUpBtn.setOnClickListener(v -> {
            isRotating = true;
            sendMessage("@DCM,R," + currentSpeed + "#");
        });

        mDownBtn.setOnClickListener(v -> {
            isRotating = true;
            sendMessage("@DCM,L," + currentSpeed + "#");
        });

        mStopBtn.setOnClickListener(v -> {
            isRotating = false;
            sendMessage("@DCM,S,0#");
        });

        mDecreaseBtn.setOnClickListener(v -> {
            currentSpeed = Math.max(0, currentSpeed - 10);
            updateSpeedDisplay();
            if (isRotating) {
                sendMessage("@DCM," + (mUpBtn.isPressed() ? "R," : "L,") + currentSpeed + "#");
            }
        });

        mIncreaseBtn.setOnClickListener(v -> {
            currentSpeed = Math.min(100, currentSpeed + 10);
            updateSpeedDisplay();
            if (isRotating) {
                sendMessage("@DCM," + (mUpBtn.isPressed() ? "R," : "L,") + currentSpeed + "#");
            }
        });

        mBTService = new BTService(this, mHandler);
        updateSpeedDisplay();
    }

    private void updateSpeedDisplay() {
        mSpeedTextView.setText("현재 속도: " + currentSpeed);
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

    @Override
    public void onStart() {
        super.onStart();
        if (!mBluetoothAdapter.isEnabled()) {
            startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), REQUEST_ENABLE_BT);
        } else if (mBTService == null) {
            initUI();
        }
    }

    @Override
    public synchronized void onResume() {
        super.onResume();
        if (mBTService != null && mBTService.getState() == BTService.STATE_NONE) {
            mBTService.start();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mBTService != null) mBTService.stop();
        if (mProgress != null) mProgress.dismiss();
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Constant.MESSAGE_STATE_CHANGE:
                    switch (msg.arg1) {
                        case BTService.STATE_CONNECTED:
                            if (mProgress != null) mProgress.dismiss();
                            break;
                        case BTService.STATE_CONNECTING:
                            showProgressDialog("Bluetooth");
                            break;
                    }
                    break;

                case Constant.MESSAGE_READ:
                    byte[] readBuf = (byte[]) msg.obj;
                    String readMessage = new String(readBuf, 0, msg.arg1);
                    // Handle incoming messages if needed
                    break;

                case Constant.MESSAGE_DEVICE_NAME:
                    mConnectedDeviceName = msg.getData().getString(Constant.DEVICE_NAME);
                    Toast.makeText(getApplicationContext(), "Connected to " + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                    break;

                case Constant.MESSAGE_TOAST:
                    if (msg.getData().getInt(Constant.TOAST) == R.string.unable_connected) {
                        if (mProgress != null) mProgress.dismiss();
                    }
                    Toast.makeText(getApplicationContext(), msg.getData().getInt(Constant.TOAST), Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    private void showProgressDialog(String type) {
        mProgress = new ProgressDialog(this);
        mProgress.setCancelable(false);
        mProgress.setMessage(" [ " + type + " ] " + getString(R.string.title_connecting));
        mProgress.show();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE:
                if (resultCode == Activity.RESULT_OK) {
                    String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                    BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
                    mBTService.connect(device);
                }
                break;

            case REQUEST_ENABLE_BT:
                if (resultCode == Activity.RESULT_OK) {
                    initUI();
                } else {
                    Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                    finish();
                }
                break;
        }
    }
}