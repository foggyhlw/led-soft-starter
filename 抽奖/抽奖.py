import asyncio
from bilibili_api import comment, video
import pprint
import csv
from bilibili_api import user
import random
pp = pprint.PrettyPrinter(indent=1)
members = set()
async def main():

    # 实例化 Video 类
    v = video.Video(bvid="BV1Nd4y1Y7Dc")
    # 获取信息
    info = await v.get_info()
    # 打印信息
    # pp.pprint(info)
    print("视频播放量为",info['stat']['view'],"次")
    random_seed = info['stat']['view']
    
    # 存储评论
    comments = []
    # 页码
    page = 1
    # 当前已获取数量
    count = 0
    while True:
        # 获取评论
        c = await comment.get_comments(v.get_aid(), comment.ResourceType.VIDEO, page)
        # 存储评论
        comments.extend(c['replies'])
        # 增加已获取数量
        count += c['page']['size']
        # 增加页码
        page += 1

        if count >= c['page']['count']:
            # 当前已获取数量已达到评论总数，跳出循环
            break
    # 打印评论
    for cmt in comments:
        # print(f"{cmt['member']['uname']}: {cmt['content']['message']}")
        members.add(cmt['member']['uname'])
    with open('comment_id.csv', 'w') as csvfile:
        for cmt in comments:
            csvfile.write(cmt['member']['uname'])
            csvfile.write('\n')

    lucky_member = list(members)
    pp.pprint(lucky_member)
    # 打印评论总数
    print(f"\n\n共有 {len(lucky_member)} 条评论（不含子评论）")

    random.seed(random_seed)
    for luck in range(3):
        print(lucky_member[random.randint(1,len(lucky_member))])

if __name__ == '__main__':
    asyncio.get_event_loop().run_until_complete(main())