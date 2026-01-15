This is a KTV application developed based on a Linux system, utilizing the open-source Freeverb for reverb effects. For the architecture diagram, please refer to karaok_architecture.jpg.  As I am using an Ubuntu virtual machine, I found that no matter how I adjust the DMA buffer, it remains the same, resulting in relatively high overall latency. If you are using an actual embedded system,  you can adjust the buffer size to achieve the desired latency. Additionally, by tuning the reverb parameters, you can obtain the desired sound effects. If you have any questions, feel free to discuss via email.  
My email address is qinbaoqiang321@163.com.


这是基于Linux系统开发的一个KTV应用，使用的是开源的freeverb做混响效果，架构图见karaok architechture.jpg 因为我用的是ubuntu虚拟机，发现不管怎么改DMA buf都是一样的，所以整体延时比较大，如果你用的是真正的嵌入式系统，可以调整一下buf size，应该可以得到理想的latency，同时通过tuning reverb的参数，得到你想要的音效。如有疑问，可以来信讨论。
我的邮箱： qinbaoqiang321@163.com
