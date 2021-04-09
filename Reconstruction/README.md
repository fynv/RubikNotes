# Reconstruction Maintenance Test

<script type="text/javascript">
var request = new XMLHttpRequest();
request.open('GET', "database.json?cb=" + new Date().getTime(), false);
request.send(null);

window.data = JSON.parse(request.responseText);

function item_select()
{
    var list = document.getElementById('list');
    var view = document.getElementById('view');
    var item = window.data[list.selectedIndex];
    var src = "https://cubing.github.io/AnimCubeJS/cube3.html?colorscheme=ywrobg&repeat=0&edit=0&movetext=1&metric=2&fonttype=0&snap=1&buttonheight=25&yz=1";
    if ("initmove" in item)
    {
        src += "&initmove="+item.initmove;
    }
    else if ("initrevmove" in item)
    {
        src += "&initrevmove="+item.initrevmove;
    }   
    src += "&move="+item.move;

    view.src = src;

    var para_comment = document.getElementById('comment');
    var comment = "";
    if ("comment" in item)
    {
        comment = item.comment;
    }
    para_comment.innerHTML=comment;
}


</script>

<div style = "display: flex; flex-wrap: wrap;" >
<iframe id="view" src="https://cubing.github.io/AnimCubeJS/cube3.html?colorscheme=ywrobg&repeat=0&edit=0&movetext=1&metric=2&fonttype=0&snap=1&buttonheight=25&yz=1" frameborder="0" width="400" height="400"></iframe>

<div style = "margin:5px;">
    <p id="comment"></p>
    <p>
        <select id="list" size = "12" ondblclick="item_select()" style="width: 300px;">
            <script type="text/javascript">
            for (var item of window.data)
            {
                document.write("<option>"+item.display_name+"</option>");
            }
            </script>
        </select>
    </p>
    <p>
        <input type="button" onclick="item_select()" value="Select"/>
        <script type="text/javascript">
        var url=window.location.href;
        var sp = url.split("/");
        var domain = sp[2];
        var repo = sp[3];
        var dir = sp[4];
        var sp2 = domain.split(".");
        var user = sp2[0];
        document.write("<input type=\"button\" onclick=\"location.href='https://github.com/"+user+"/"+repo+"/edit/master/"+dir+"/database.json';\" value=\"Edit\"/>");
        </script>
    </p>
</div>

</div>

