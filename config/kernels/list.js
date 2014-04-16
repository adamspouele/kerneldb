({
	"name": "List",
	"author": "subprotocol",
	"description": "Maintains a list of values, where the max length is determined at query time.",
	"merge": function(key, old, value, log) {
		return (old||[]).concat(value["value"]).slice(-value["size"]);
	}
});