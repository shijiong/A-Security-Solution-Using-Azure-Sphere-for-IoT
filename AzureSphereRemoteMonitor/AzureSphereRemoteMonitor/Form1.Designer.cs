namespace AzureSphereRemoteMonitor
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.humid = new System.Windows.Forms.Label();
            this.ambiTemp = new System.Windows.Forms.Label();
            this.Title = new System.Windows.Forms.Label();
            this.temperature = new System.Windows.Forms.Label();
            this.humi = new System.Windows.Forms.Label();
            this.TempTh = new System.Windows.Forms.Label();
            this.TempThreshold = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.sysstatus = new System.Windows.Forms.Label();
            this.SetTemp = new System.Windows.Forms.Button();
            this.msgt = new System.Windows.Forms.Label();
            this.msgTime = new System.Windows.Forms.Label();
            this.light = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.sound = new System.Windows.Forms.Label();
            this.gas = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // humid
            // 
            this.humid.AutoSize = true;
            this.humid.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.humid.ForeColor = System.Drawing.Color.Black;
            this.humid.Location = new System.Drawing.Point(65, 301);
            this.humid.Name = "humid";
            this.humid.Size = new System.Drawing.Size(191, 32);
            this.humid.TabIndex = 15;
            this.humid.Text = "Humidity (%) :";
            // 
            // ambiTemp
            // 
            this.ambiTemp.AutoSize = true;
            this.ambiTemp.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ambiTemp.ForeColor = System.Drawing.Color.Black;
            this.ambiTemp.Location = new System.Drawing.Point(61, 251);
            this.ambiTemp.Name = "ambiTemp";
            this.ambiTemp.Size = new System.Drawing.Size(237, 32);
            this.ambiTemp.TabIndex = 14;
            this.ambiTemp.Text = "Temperature (C) :";
            // 
            // Title
            // 
            this.Title.AutoSize = true;
            this.Title.Font = new System.Drawing.Font("Microsoft Sans Serif", 19.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Title.ForeColor = System.Drawing.Color.Black;
            this.Title.Location = new System.Drawing.Point(343, 23);
            this.Title.Name = "Title";
            this.Title.Size = new System.Drawing.Size(559, 46);
            this.Title.TabIndex = 0;
            this.Title.Text = "Azure Sphere Remote Monitor";
            // 
            // temperature
            // 
            this.temperature.AutoSize = true;
            this.temperature.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.temperature.ForeColor = System.Drawing.Color.Black;
            this.temperature.Location = new System.Drawing.Point(309, 249);
            this.temperature.Name = "temperature";
            this.temperature.Size = new System.Drawing.Size(61, 32);
            this.temperature.TabIndex = 29;
            this.temperature.Text = "null";
            // 
            // humi
            // 
            this.humi.AutoSize = true;
            this.humi.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.humi.ForeColor = System.Drawing.Color.Black;
            this.humi.Location = new System.Drawing.Point(309, 301);
            this.humi.Name = "humi";
            this.humi.Size = new System.Drawing.Size(61, 32);
            this.humi.TabIndex = 30;
            this.humi.Text = "null";
            // 
            // TempTh
            // 
            this.TempTh.AutoSize = true;
            this.TempTh.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TempTh.ForeColor = System.Drawing.Color.Black;
            this.TempTh.Location = new System.Drawing.Point(556, 407);
            this.TempTh.Name = "TempTh";
            this.TempTh.Size = new System.Drawing.Size(371, 32);
            this.TempTh.TabIndex = 31;
            this.TempTh.Text = "Temperature Threshold (C) :";
            // 
            // TempThreshold
            // 
            this.TempThreshold.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TempThreshold.Location = new System.Drawing.Point(933, 404);
            this.TempThreshold.Name = "TempThreshold";
            this.TempThreshold.Size = new System.Drawing.Size(112, 39);
            this.TempThreshold.TabIndex = 32;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(65, 407);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(191, 32);
            this.label1.TabIndex = 33;
            this.label1.Text = "Relay Status :";
            // 
            // sysstatus
            // 
            this.sysstatus.AutoSize = true;
            this.sysstatus.BackColor = System.Drawing.Color.Silver;
            this.sysstatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.sysstatus.ForeColor = System.Drawing.Color.Teal;
            this.sysstatus.Location = new System.Drawing.Point(313, 407);
            this.sysstatus.Name = "sysstatus";
            this.sysstatus.Size = new System.Drawing.Size(74, 32);
            this.sysstatus.TabIndex = 34;
            this.sysstatus.Text = "OFF";
            // 
            // SetTemp
            // 
            this.SetTemp.BackColor = System.Drawing.Color.Yellow;
            this.SetTemp.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SetTemp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
            this.SetTemp.Location = new System.Drawing.Point(1072, 403);
            this.SetTemp.Name = "SetTemp";
            this.SetTemp.Size = new System.Drawing.Size(89, 40);
            this.SetTemp.TabIndex = 35;
            this.SetTemp.Text = "Set";
            this.SetTemp.UseVisualStyleBackColor = false;
            this.SetTemp.Click += new System.EventHandler(this.SetTemp_Click);
            // 
            // msgt
            // 
            this.msgt.AutoSize = true;
            this.msgt.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.msgt.ForeColor = System.Drawing.Color.Black;
            this.msgt.Location = new System.Drawing.Point(65, 351);
            this.msgt.Name = "msgt";
            this.msgt.Size = new System.Drawing.Size(208, 32);
            this.msgt.TabIndex = 36;
            this.msgt.Text = "Message Time:";
            // 
            // msgTime
            // 
            this.msgTime.AutoSize = true;
            this.msgTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.msgTime.ForeColor = System.Drawing.Color.Black;
            this.msgTime.Location = new System.Drawing.Point(309, 351);
            this.msgTime.Name = "msgTime";
            this.msgTime.Size = new System.Drawing.Size(61, 32);
            this.msgTime.TabIndex = 37;
            this.msgTime.Text = "null";
            // 
            // light
            // 
            this.light.AutoSize = true;
            this.light.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.light.ForeColor = System.Drawing.Color.Black;
            this.light.Location = new System.Drawing.Point(309, 199);
            this.light.Name = "light";
            this.light.Size = new System.Drawing.Size(61, 32);
            this.light.TabIndex = 39;
            this.light.Text = "null";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.Black;
            this.label3.Location = new System.Drawing.Point(63, 199);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(93, 32);
            this.label3.TabIndex = 38;
            this.label3.Text = "Light :";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.ForeColor = System.Drawing.Color.Black;
            this.label5.Location = new System.Drawing.Point(62, 148);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(113, 32);
            this.label5.TabIndex = 40;
            this.label5.Text = "Sound :";
            // 
            // sound
            // 
            this.sound.AutoSize = true;
            this.sound.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.sound.ForeColor = System.Drawing.Color.Black;
            this.sound.Location = new System.Drawing.Point(309, 148);
            this.sound.Name = "sound";
            this.sound.Size = new System.Drawing.Size(61, 32);
            this.sound.TabIndex = 41;
            this.sound.Text = "null";
            // 
            // gas
            // 
            this.gas.AutoSize = true;
            this.gas.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gas.ForeColor = System.Drawing.Color.Black;
            this.gas.Location = new System.Drawing.Point(308, 96);
            this.gas.Name = "gas";
            this.gas.Size = new System.Drawing.Size(61, 32);
            this.gas.TabIndex = 43;
            this.gas.Text = "null";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 13.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.ForeColor = System.Drawing.Color.Black;
            this.label4.Location = new System.Drawing.Point(61, 95);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(82, 32);
            this.label4.TabIndex = 42;
            this.label4.Text = "Gas :";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(625, 95);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(537, 288);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 44;
            this.pictureBox1.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1223, 535);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.gas);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.sound);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.light);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.msgTime);
            this.Controls.Add(this.msgt);
            this.Controls.Add(this.SetTemp);
            this.Controls.Add(this.sysstatus);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.TempThreshold);
            this.Controls.Add(this.TempTh);
            this.Controls.Add(this.humi);
            this.Controls.Add(this.temperature);
            this.Controls.Add(this.humid);
            this.Controls.Add(this.ambiTemp);
            this.Controls.Add(this.Title);
            this.Name = "Form1";
            this.Text = "Azure Sphere Desktop";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label Title;
        private System.Windows.Forms.Label ambiTemp;
        private System.Windows.Forms.Label humid;
        private System.Windows.Forms.Label temperature;
        private System.Windows.Forms.Label humi;
        private System.Windows.Forms.Label TempTh;
        private System.Windows.Forms.TextBox TempThreshold;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label sysstatus;
        private System.Windows.Forms.Button SetTemp;
        private System.Windows.Forms.Label msgt;
        private System.Windows.Forms.Label msgTime;
        private System.Windows.Forms.Label light;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label sound;
        private System.Windows.Forms.Label gas;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

