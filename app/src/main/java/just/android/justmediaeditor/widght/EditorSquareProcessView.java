package just.android.justmediaeditor.widght;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;
import just.android.justmediaeditor.R;
import just.android.justmediaeditor.util.CommonUtil;

public class EditorSquareProcessView extends View {
    private static final int MAX_PROGRESS = 100;
    private static final float PER_LINE_MAX_PROCESS = 100F / 4;

    private final Context mContext;
    private Paint paint;
    private Paint processPaint;
    private Paint textPaint;
    private Canvas canvas;

    private float currentPogress;
    private int strokeColor = Color.BLACK;
    private float strokeWith = 5.0f;
    private int progressColor = Color.RED;
    private int textColor = Color.BLUE;
    private float textSize = 10.0f;

    public EditorSquareProcessView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        initValue(attrs);
    }


    private void initValue(AttributeSet attrs) {
        TypedArray typedArray = mContext.obtainStyledAttributes(attrs, R.styleable.SquareProcessView);
        strokeColor = typedArray.getColor(R.styleable.SquareProcessView_strokeColor, Color.GRAY);
        strokeWith = typedArray.getDimension(R.styleable.SquareProcessView_strokeWith, strokeWith);
        progressColor = typedArray.getColor(R.styleable.SquareProcessView_progressColor, Color.RED);
        textColor = typedArray.getColor(R.styleable.SquareProcessView_textColor, Color.BLACK);
        textSize = typedArray.getDimension(R.styleable.SquareProcessView_textSize, textSize);
        initPaints();
    }

    private void initPaints() {
        paint = new Paint();
        paint.setColor(strokeColor);
        paint.setStrokeWidth(CommonUtil.dp2px(getContext(), strokeWith));
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.STROKE);
        initProcessPaint();
        initTextPaint();
    }

    private void initProcessPaint() {
        processPaint = new Paint();
        processPaint.setColor(progressColor);
        processPaint.setStrokeWidth(CommonUtil.dp2px(getContext(), strokeWith));
        processPaint.setAntiAlias(true);
        processPaint.setStyle(Paint.Style.STROKE);
    }

    private void initTextPaint() {
        textPaint = new Paint();
        textPaint.setColor(textColor);
        textPaint.setAntiAlias(true);
        textPaint.setStyle(Paint.Style.STROKE);
        textPaint.setTextSize(textSize);

    }

    public void setCurrentPogress(float progress) {
        if (progress > currentPogress && progress <= 100) {
            this.currentPogress = progress;
            invalidate();
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        this.canvas = canvas;
        drawSquare();
        drawProcessSquare(currentPogress);
        drawPercent();
    }

    /**
     * 四条线组成一个方形
     */
    private void drawSquare() {
        drawTopLine();
        drawRightLine();
        drawBottomLine();
        drawLeftLine();
    }


    private void drawTopLine() {
        Path path = new Path();
        path.moveTo(0, 0);
        path.lineTo(canvas.getWidth(), 0);
        canvas.drawPath(path, paint);

    }

    private void drawRightLine() {
        Path path = new Path();
        path.moveTo(canvas.getWidth(), 0);
        path.lineTo(canvas.getWidth(), canvas.getHeight());
        canvas.drawPath(path, paint);
    }

    private void drawBottomLine() {
        Path path = new Path();
        path.moveTo(canvas.getWidth(), canvas.getHeight());
        path.lineTo(0, canvas.getHeight());
        canvas.drawPath(path, paint);
    }

    private void drawLeftLine() {
        Path path = new Path();
        path.moveTo(0, 0);
        path.lineTo(0, canvas.getHeight());
        canvas.drawPath(path, paint);
    }


    /**
     * 画进度
     */
    private void drawProcessSquare(float progress) {
        float topProcess = 0;
        float rightProcess = 0;
        float bottomProcess = 0;
        float leftProcess = 0;
        if (progress <= PER_LINE_MAX_PROCESS) {
            topProcess = progress;
        } else if (progress <= PER_LINE_MAX_PROCESS * 2) {
            topProcess = PER_LINE_MAX_PROCESS;
            rightProcess = progress - PER_LINE_MAX_PROCESS;
        } else if (progress <= PER_LINE_MAX_PROCESS * 3) {
            topProcess = PER_LINE_MAX_PROCESS;
            rightProcess = PER_LINE_MAX_PROCESS;
            bottomProcess = progress - PER_LINE_MAX_PROCESS * 2;
        } else if (progress <= MAX_PROGRESS) {
            topProcess = PER_LINE_MAX_PROCESS;
            rightProcess = PER_LINE_MAX_PROCESS;
            bottomProcess = PER_LINE_MAX_PROCESS;
            leftProcess = progress - PER_LINE_MAX_PROCESS * 3;
        }
        drawProgressTopLine(topProcess);
        drawProgressRightLine(rightProcess);
        drawProgressBottomLine(bottomProcess);
        drawProgressLeftLine(leftProcess);
    }

    private void drawProgressTopLine(float progress) {
        Path path = new Path();
        path.moveTo(0, 0);
        path.lineTo(canvas.getWidth() / PER_LINE_MAX_PROCESS * progress, 0);
        canvas.drawPath(path, processPaint);
    }

    private void drawProgressRightLine(float progress) {
        Path path = new Path();
        path.moveTo(canvas.getWidth(), 0);
        path.lineTo(canvas.getWidth(), canvas.getHeight() / PER_LINE_MAX_PROCESS * progress);
        canvas.drawPath(path, processPaint);
    }

    private void drawProgressBottomLine(float progress) {
        Path path = new Path();
        path.moveTo(canvas.getWidth(), canvas.getHeight());
        path.lineTo(canvas.getWidth() / PER_LINE_MAX_PROCESS * Math.abs(progress - PER_LINE_MAX_PROCESS), canvas.getHeight());
        canvas.drawPath(path, processPaint);
    }

    private void drawProgressLeftLine(float progress) {
        Path path = new Path();
        path.moveTo(0, canvas.getHeight());
        path.lineTo(0, canvas.getHeight() / PER_LINE_MAX_PROCESS * Math.abs(progress - PER_LINE_MAX_PROCESS));
        canvas.drawPath(path, processPaint);
    }

    private void drawPercent() {
        int width = canvas.getWidth();
        int height = canvas.getHeight();
        textPaint.setTextAlign(Paint.Align.CENTER);
        Paint.FontMetrics fontMetrics = textPaint.getFontMetrics();
        float top = fontMetrics.top;
        float bottom = fontMetrics.bottom;
        canvas.drawText(String.format(getContext().getString(R.string.progress_value), currentPogress) + "%",
                width / 2, height / 2 - top / 2 - bottom / 2, textPaint);
    }
}
