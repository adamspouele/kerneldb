({
	"name": "Counter",
	"author": "subprotocol",
	"description": "Increments old value by new value.",
	"merge": function(key, old, value) {
		return (old||0) + value;
	},
	"partialMerge": function(key, left, right) {
		return left + right;
	}
});