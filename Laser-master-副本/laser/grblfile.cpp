#include "grblfile.h"
#include <QFile>

GrblFile::GrblFile(QWidget *parent) : QWidget(parent)
{
    GCODE_STD_HEADER = "G90 (use absolute coordinates)";
    GCODE_STD_FOOTER = "G0 X0 Y0 Z0 (move back to origin)";
}


//void GrblFile::SaveProgram(QString savepath, bool header, bool footer, bool between, int cycles)
//{
//    try
//    {
//        if(!savepath.isEmpty())
//        {
//            QFile sw;
//            //关联文件对象
//            sw.setFileName(savepath);
//            //以只写方式打开文件
//           if(sw.open(QIODevice::WriteOnly))
//           {
//               if (header)
//                   //写入文件头信息，  GCODE_STD_HEADER = "G90 (use absolute coordinates)";
//                   //QString to QByteArray
//                   sw.write(GCODE_STD_HEADER.toUtf8());

//               //            for (int i = 0; i < cycles; i++)
//               //            {
//               //                //遍历出list中的每条Grbl命令
//               //                foreach (GrblCommand cmd in list)
//               //                    sw.WriteLine(cmd.Command);

//               //                //bool between, between && i结果为1， 意思是循环到cycles-1=0， 即cycles=1时
//               //                //写入GCODE_STD_PASSES = ";(Uncomment if you want to sink Z axis)\r\n;G91 (use relative coordinates)\r\n;G0 Z-1 (sinks the Z axis, 1mm)\r\n;G90 (use absolute coordinates)";
//               //                if (between && i < cycles-1)
//               //                    sw.WriteLine(Settings.GetObject("GCode.CustomPasses", GrblCore.GCODE_STD_PASSES));
//               //            }

//               if (footer)
//                   //写入文件尾部信息， GCODE_STD_FOOTER = "G0 X0 Y0 Z0 (move back to origin)";
//                   sw.write(GCODE_STD_FOOTER.toUtf8());
//           }
//           sw.close();
//        }
//    }
//    catch(std::exception) { }
//}
