{*
 * $Revision: 16290 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Recent Events"} </h2>
</div>

<div id="eventBlock" class="gbBlock" style="display: none">
  <table class="gbDataTable">
    <thead>
      <tr>
	<th>
	  {g->text text="Date"}
	</th>
	<th>
	  {g->text text="Type"}
	</th>
	<th>
	  {g->text text="Location"}
	</th>
	<th>
	  {g->text text="Client"}
	</th>
	<th>
	  {g->text text="Summary"}
	</th>
      </tr>
    </thead>
    <tbody id="eventTableBody">
    </tbody>
  </table>

  <div style="margin-bottom: 10px" class="gcBackground1">
    <span id="pageMessage" style="display: none"></span>
    <span id="previousPage" style="display: none; margin: 10px"><a href="#" onclick="javascript:changePage(-1)">{g->text text="&laquo; back"}</a></span>
    <span id="nextPage" style="display: none; padding: 10px"><a href="#" onclick="javascript:changePage(+1)">{g->text text="next &raquo;"}</a></span>
  </div>
</div>

<div class="gbBlock" id="noEventsBlock" style="display: none">
  <h2> {g->text text="No events have been reported yet."} </h2>
</div>

<div id="eventDetails" class="gbBlock" style="display: none">
  <h2> {g->text text="Event Details"} </h2>
  <table class="gbDataTable">
    <tr> <td class="gbOdd"> {g->text text="Date"} </td> <td class="gbEven" id="dateDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Type"} </td> <td class="gbEven" id="typeDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Location"} </td> <td class="gbEven" id="locationDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="User Id"} </td> <td class="gbEven" id="userIdDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Client"} </td> <td class="gbEven" id="clientDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Summary"} </td> <td class="gbEven" id="summaryDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Referer"} </td> <td class="gbEven" id="refererDetails"> </td> </tr>
    <tr> <td class="gbOdd"> {g->text text="Details"} </td> <td class="gbEven"> <pre> <span id="detailsDetails"> </span></pre> </td> </tr>
  </table>
</div>

<script type="text/javascript">
// <![CDATA[
var getRecordsUrl = '{g->url arg1="view=core.AdminEventLogViewerCallback" arg2="command=getRecords" arg3="pageNumber=__PAGE_NUMBER__" arg4="pageSize=__PAGE_SIZE__" useAuthToken=1 htmlEntities=0}';
var getRecordDetailsUrl = '{g->url arg1="view=core.AdminEventLogViewerCallback" arg2="command=getRecordDetails" arg3="id=__ID__" useAuthToken=1 htmlEntities=0}';
var pageNumber = 1;
var pageSize = 20;
var totalPages = 1;
var noSummaryText = '{g->text text="no summary" forJavascript=1}';
var locationLink = '<a href="__PLACEHOLDER__">{g->text text="Link" forJavascript=1}</a>';
var failureMessage = '{g->text text="An error occurred while retrieving the event log entry details." forJavascript=1}';

{literal}
var eventTableBody = document.getElementById("eventTableBody");
function setRecord(index, record) {
  var row;
  if (eventTableBody.rows.length <= index) {
    row = eventTableBody.appendChild(document.createElement("tr"));
    YAHOO.util.Dom.addClass(row, index % 2 ? "gbOdd" : "gbEven");
    for (var i = 0; i < 5; i++) {
      row.appendChild(document.createElement("td"));
    }
  } else {
    row = eventTableBody.rows[index];
  }
  var i = 0;
  row.childNodes[i++].innerHTML = record['date'].replace(/ /g, '&nbsp;');
  row.childNodes[i++].innerHTML = record['type'].replace(/ /g, '&nbsp;');
  row.childNodes[i++].innerHTML = locationLink.replace('__PLACEHOLDER__', record['location']);
  row.childNodes[i++].innerHTML = record['client'];
  var summary = record['summary'] ? record['summary'] : '<i>' + noSummaryText + '</i>';
  row.childNodes[i++].innerHTML =
    '<a href="javascript:getRecordDetails(' + record['id'] + ')">' + summary + '</a>';
}

function changePage(deltaPages) {
  var newPageNumber = Math.min(Math.max(pageNumber + deltaPages, 1), totalPages);
  if (newPageNumber != pageNumber) {
    pageNumber = newPageNumber;
    getRecords();
  }
}

function getRecordDetails(id) {
  var url = getRecordDetailsUrl.replace('__ID__', id);
  YAHOO.util.Connect.asyncRequest('GET', url, {
	"success": displayRecordDetails,
	"failure": function() { alert(failureMessage); }
	});
}

function displayRecordDetails(response) {
  var data = eval("(" + response.responseText + ")");
  for (var key in data) {
    var el = document.getElementById(key + "Details");
    if (el) {
      el.innerHTML = data[key];
    }
  }
  document.getElementById("eventDetails").style.display = "block";
}

function getRecords() {
  var url = getRecordsUrl.replace('__PAGE_NUMBER__', pageNumber).replace('__PAGE_SIZE__', pageSize);
  YAHOO.util.Connect.asyncRequest('GET', url, {"success":  displayRecords});
}

function displayRecords(response) {
  var data = eval("(" + response.responseText + ")");
  for (var i = 0; i < data.records.length; i++) {
    setRecord(i, data.records[i]);
  }
  while (eventTableBody.rows.length > data.records.length) {
    eventTableBody.removeChild(eventTableBody.rows[eventTableBody.rows.length - 1]);
  }
  document.getElementById("pageMessage").innerHTML = data.pageMessage;
  totalPages = data.totalPages;
  document.getElementById("previousPage").style.display = (pageNumber == 1) ? "none" : "inline";
  document.getElementById("nextPage").style.display = (pageNumber == totalPages) ? "none" : "inline";

  document.getElementById("eventBlock").style.display = totalPages > 0 ? "block" : "none";
  document.getElementById("noEventsBlock").style.display = totalPages > 0 ? "none" : "block";
}

getRecords();
{/literal}
// ]]>
</script>
