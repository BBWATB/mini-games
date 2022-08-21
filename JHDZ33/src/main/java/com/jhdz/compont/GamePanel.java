package com.jhdz.compont;

import javax.swing.*;
import java.awt.*;

public class GamePanel extends JFrame {

    int width = 800;
    int height = 610;
    public void init() {
        // 设置窗口名
        setTitle("坦克大戰試做型");
        // 设置窗口大小
        setSize(width,height);
        // 设置初始化屏幕居中
        setLocationRelativeTo(null);
        // 设置关闭事件
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        // 设置窗口不可调整大小
        setResizable(false);
        // 设置窗口可见
        setVisible(true);
        // 添加键盘监视器
        this.addKeyListener(new KeyMonitor());
    }

    @Override
    public void paint(Graphics graphics) {
        // 设置图样颜色
        graphics.setColor(Color.gray);
        // 设置图样大小尺寸
        graphics.fillRect(0,0,width,height);
        // 改变画笔颜色
        graphics.setColor(Color.orange);
        // 设置文字样式和尺寸：仿宋，粗体，50
        graphics.setFont(new Font("仿宋",Font.BOLD,50));
        graphics.drawString("游戏模式",220,300);
        graphics.drawString("x模式",220,400);
        graphics.drawString("y模式",220,500);

    }

}
