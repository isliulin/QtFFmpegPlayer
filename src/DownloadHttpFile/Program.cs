using System;
using System.IO;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;

namespace GetFFmpegDLL
{
    class Program
    {
        static void Main(string[] args)
        {
            string httpurl = "";
            string localurl = "";
            if(args.Length == 2)
            {
                httpurl = args[0];
                localurl = args[1];
            }
            if(string.IsNullOrEmpty(httpurl) || string.IsNullOrEmpty(localurl))
            {
                Console.WriteLine("地址无效");
                return;
            }
            Util.SetCertificatePolicy();
            DownloadHttpFile(httpurl, localurl);
        }

        private static void DownloadHttpFile(string httpUrl, string saveUrl)
        {
            if(File.Exists(saveUrl))
            {
                Console.WriteLine("文件存在，无需下载");
                return;
            }
            try
            {
                long t = GetTimestamp();
                WebResponse response = null;
                //获取远程文件
                WebRequest request = WebRequest.Create(httpUrl);
                response = request.GetResponse();
                if (response == null) return;
                //读远程文件的大小
                long fileSize = response.ContentLength;
                Console.WriteLine("文件大小:" + fileSize);

                //下载远程文件
                long crtPos = 0;
                Stream netStream = response.GetResponseStream();
                FileStream fileStream = new FileStream(saveUrl, FileMode.Create);
                byte[] read = new byte[10240];

                int realReadLen = netStream.Read(read, 0, read.Length);
                crtPos += realReadLen;
                int lastPercent = 0;
                while (realReadLen > 0)
                {
                    int percent = (int)((crtPos * 1.0f / fileSize) * 100);
                    if(percent != lastPercent)
                    {
                        Console.WriteLine(string.Format("下载进度:{0}%", percent));
                        lastPercent = percent;
                    }
                    
                    try
                    {
                        fileStream.Write(read, 0, realReadLen);
                        fileStream.Flush();

                        netStream.ReadTimeout = 2000;
                        realReadLen = netStream.Read(read, 0, read.Length);
                        crtPos += realReadLen;
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex);
                        Console.Read();
                    }
                }
                long usetime = GetTimestamp() - t;
                Console.WriteLine(string.Format("下载速度:{0:F2}kb/s", (fileSize * 1.0f / usetime)));
                Console.WriteLine("下载完毕");
                response.Close();
                netStream.Close();
                fileStream.Close();
            }
            catch (Exception ex) {
                Console.WriteLine(ex);
            }
        }

        public static long GetTimestamp()
        {
            //获取从1970年一月一日0点0分0秒0微妙开始
            return (DateTime.Now.Ticks - new DateTime(1970, 1, 1, 0, 0, 0, 0).Ticks) / 10000;
        }
    }
}
public static class Util
{
    /// <summary>
    /// 设置cert策略
    /// </summary>
    public static void SetCertificatePolicy()
    {
        ServicePointManager.ServerCertificateValidationCallback += RemoteCertificateValidate;
    }

    /// <summary>
    /// 远程证书验证
    /// </summary>
    private static bool RemoteCertificateValidate(object sender, X509Certificate cert, X509Chain chain, SslPolicyErrors error)
    {
        // 相信任何证书!!!
        Console.WriteLine("相信任何证书");
        return true;
    }
}