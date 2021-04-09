# Reconstruction Maintenance Test

<script type="text/javascript">
var request = new XMLHttpRequest();
request.open('GET', "database.json", false);
request.send(null);

window.data = JSON.parse(request.responseText);

function item_select()
{
	var list = document.getElementById('list');
	var view = document.getElementById('view');
	var item = window.data[list.selectedIndex];
	view.src = "https://cubing.github.io/AnimCubeJS/cube3.html?colorscheme=ywrobg&initrevmove="+item.initrevmove+"&move="+item.move+"&repeat=0&edit=0&movetext=1&metric=2&fonttype=0&snap=1&buttonheight=20&yz=1";
}


</script>

<div style = "display: flex; flex-wrap: wrap;" >
<iframe id="view" src="https://cubing.github.io/AnimCubeJS/cube3.html?colorscheme=ywrobg&repeat=0&edit=0&movetext=1&metric=2&fonttype=0&snap=1&buttonheight=30&yz=1" frameborder="0" width="400" height="400"></iframe>


<select id="list" size = "15" ondblclick="item_select()">
	<script type="text/javascript">
		for (var item of window.data)
		{
			document.write("<option>"+item.display_name+"</option>");
		}
	</script>
</select>

</div>

