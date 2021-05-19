$("#submit_login_authorize").on("click", async () => {
	await execute_login();
});

$("#submit_register_authorize").on("click", async () => {
	await execute_login(undefined, undefined, true);
});

$("#submit_offline_authorize").on("click", async () => {
	await execute_offline_login();
});

$("#bot_start_button").on("click", async () => {
   await update_bot_start_button(true);
});

$("#records_table_update_button").on("click", async () => {
    await update_records();
});

$("#user_records_table_update_button").on("click", async () => {
    await update_user_records();
});

$("#statistics_table_update_button").on("click", async () => {
    await update_statistics();
});

$("#graph_table_update_button").on("click", async () => {
    await update_graph();
});

$("#settings_table_save_button").on("click", async () => {
    await update_settings();
});

let debug = false;
let disable_f5 = !debug && true;
let disable_f11 = !debug && true;
let disable_f12 = !debug && true;
let disable_right_click = !debug && true;

async function init_debug()
{
    if (await eel.is_debug()())
    {
        debug = true;
        disable_f5 = !debug && true;
        disable_f11 = !debug && true;
        disable_f12 = !debug && true;
        disable_right_click = !debug && true;
    }

    window.addEventListener("keydown", function(event) {
        if (event.keyCode === 116 && disable_f5) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        } else if (event.keyCode === 122 && disable_f11) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        } else if (event.keyCode === 123 && disable_f12) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        } else if (event.ctrlKey && event.shiftKey && event.keyCode === 73) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        } else if (event.ctrlKey && event.shiftKey && event.keyCode === 74) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        }
    });

    document.addEventListener('contextmenu', event =>{
        if (disable_right_click)
        {
            event.preventDefault()
        }
    });
}

init_debug();


eel.expose(notify);
function notify(text, color='#FFFFFF', delay=3000)
{
    Toast.add({
            text: text,
            color: color,
            autohide: true,
            delay: delay
        });
}

eel.expose(log_notify);
function log_notify(message, msg_type=0)
{
    let text = message;
    let color;
    let autohide;
    let delay;

    switch (msg_type)
    {
        case 0:
            color = "#FFFFFF";
            autohide = true;
            delay = 5000;
            break;
        case 1:
            color = "#00FF00";
            autohide = true;
            delay = 5000;
            break;
        case 2:
            color = "#FFA000";
            autohide = true;
            delay = 10000;
            break;
        case 3:
            color = "#FF7F7F";
            autohide = false;
            delay = 20000;
            break;
        case 4:
            color = "#FF0000";
            autohide = false;
            delay = 60000;
            break;
    }

    Toast.add({
            text: text,
            color: color,
            autohide: autohide,
            delay: delay
    });
}

const ProtocolType = Object.freeze({"STRONGHTTPS":0, "HTTPS":1, "HTTP":2, "STRONGANY":3, "ANY":4})


function time_converter(timestamp)
{
    timestamp = parseInt(timestamp);
    let date = new Date(timestamp * 1000);
    let months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
    let year = date.getFullYear();
    let month = months[date.getMonth()];
    let day = date.getDate();
    let hour = date.getHours();
    let min = date.getMinutes();
    let sec = date.getSeconds();
    return day + ' ' + month + ' ' + year + ' ' + hour + ':' + min + ':' + sec;
}

try
{
    let name = window.name;

    if (name)
    {
        let json = JSON.parse(name);
        let authorized = json["authorized"];
        if (authorized)
        {
            const login = json["login"];
            const apikey = json["apikey"];

            if (apikey !== null)
            {
                execute_login(login, apikey);
            }
            else
            {
                execute_offline_login(login);
            }
        }
    }
}
catch (exception)
{
}

async function set_last_login()
{
    const host_input = $("#host_input");
    const port_input = $("#port_input");
    const login_input = $("#login_input");
    const password_input = $("#password_input")
    const https_input = $("#https_mode");

    let host = host_input.val();
    let port = port_input.val();
    let login = login_input.val();

    https_input.val(await eel.py_get_https()());

    if (!host)
    {
        host = await eel.py_get_host()();
        host_input.val(host ? host : "localhost");
    }

    if (!port)
    {
        port = await eel.py_get_port()();
        port_input.val(port ? port : 3000);
    }

    if (!login)
    {
        login = await eel.py_get_user_login()();
        if (login)
        {
            login_input.val(login);
        }
    }
}

set_last_login();

async function execute_login(login, apikey, eula)
{
    if (!login || !apikey)
    {
        const host = $("#host_input").val();
        const port = parseInt($("#port_input").val());
        login = $("#login_input").val();
        const password = $("#password_input").val();
        const https = parseInt($("#https_mode").val());

        if (!host || !port || !login || !password)
        {
            swal("One or more fields are empty!");
            return;
        }

        let login_authorize = $("#submit_login_authorize");
        let register_authorize = $("#submit_register_authorize");
        let offline_authorize = $("#submit_offline_authorize");
        login_authorize.hide();
        register_authorize.hide();
        offline_authorize.hide();

        await eel.py_init_connection(host, port, https !== ProtocolType.HTTP, https === ProtocolType.STRONGHTTPS || https === ProtocolType.STRONGANY)();
        const status = await eel.py_user_login(login.toString(), password.toString(), eula)();

        if (typeof status !== "object" && (https === ProtocolType.STRONGANY || https === ProtocolType.ANY))
        {
            await eel.py_init_connection(host, port, false, https === ProtocolType.STRONGANY)();
            const status = await eel.py_user_login(login.toString(), password.toString(), eula)();
        }

        if (typeof status !== "object")
        {
            login_authorize.show();
            register_authorize.show();
            offline_authorize.show();

            switch (status)
            {
                case 0:
                    swal("Connection error. Maybe server is offline.");
                    return;
                case 401:
                    swal("Invalid login or password");
                    return;
                case 403:
                    swal("Access restricted. Your account is banned?");
                    return;
                case 495:
                    swal("Server HTTPS certificate is invalid or self-signed.");
                    return;
                case 503:
                    swal("Internal server error.");
                    return;
                default:
                    swal(`Authorization code:${status}`);
                    return;
            }
        }

        login = status[0]
        apikey = status[1];

        await eel.py_set_host(host.toString())();
        await eel.py_set_port(port)();
        await eel.py_set_https(https)();

        window.name = JSON.stringify({
            "authorized": true,
            "login": login,
            "apikey": apikey
        });
    }

    let div = $("div").first();
	div.children("#authorize").hide();
	div.prepend(`<a id="login_label" class=\"rainbow_text_animated loginlabel\">🖧 ${login}</a>`);
	await show_page()
}

async function execute_offline_login(login)
{
    if (!login)
    {
        login = $("#login_input").val();
    }

    window.name = JSON.stringify({
            "authorized": true,
            "login": login,
            "apikey": null
    });

    let div = $("div").first();
	div.children("#authorize").hide();
	div.prepend(`<a id="login_label" class=\"offline_login_animated loginlabel\">${login}</a>`);
	await show_page(true);
}

async function update_bot_start_button(click)
{
    let button = $("#bot_start_button");

    let started = await eel.py_bot_is_started()();

    if (click)
    {
        if (started)
        {
            button.hide();
            await eel.py_bot_is_started(false)();
            button.css("background-color", "limegreen");
            button.text("Start");
            button.show();
            return false;
        }
        else
        {
            button.hide();
            await eel.py_bot_is_started(true)();
            button.css("background-color", "red");
            button.text("Stop");
            button.show();
            return true;
        }
    }
    else
    {
        if (started)
        {
            button.css("background-color", "red");
            button.text("Stop");
        }
        else
        {
            button.css("background-color", "limegreen");
            button.text("Start");
        }
    }

    return started;
}

async function update_main()
{
    await update_bot_start_button()
}

async function update_records()
{
    let start = $("#records_start_date_input").val();
    let end = $("#records_end_date_input").val();
    let take = $("#records_take_count_input").val();
    let skip = $("#records_skip_count_input").val();

    if (start)
    {
        start = parseInt((new Date(start).getTime() / 1000).toFixed(0));
    }

    if (!start)
    {
        start = 0;
    }

    if (end)
    {
        end = parseInt((new Date(end).getTime() / 1000).toFixed(0));
    }

    if (!end)
    {
        end = 0x7FFFFFFF
    }

    if (take)
    {
        take = parseInt(take);
    }

    if (!take)
    {
        take = 10;
    }

    if (skip)
    {
        skip = parseInt(skip);
    }

    if (!skip)
    {
        skip = 0;
    }

    let button = $("#records_table_update_button");
    button.hide();

    let result = await eel.py_get_records(start, end, take, skip)();
    if (typeof result !== "string")
    {
        button.show();
        switch (result)
        {
            case 0:
                swal("Connection error. Maybe server is offline.");
                return;
            case 401:
                swal("Invalid apikey")
                return;
            case 403:
                swal("Access restricted. Your account is banned?")
                return;
            case 503:
                swal("Internal server error")
                return;
            default:
                swal(`Request status code:${result}`);
                return;
        }
    }

    let records = JSON.parse(result);

    let table = $("#records_table");
    let tr = $("#records_table tr");
    for (let i = 1; i < tr.length; i++)
    {
        tr[i].remove();
    }

    let i = 0;
	for (let value of records)
	{
	    let style;
	    switch (i)
        {
            case 0:
                style = " class=\"rainbow_text_animated\" style=\"font-size: 30px; font-weight: bold;\"";
                break;
            case 1:
                style = " class=\"gold_text_animated\" style=\"font-size: 29.5px; font-weight: bold;\"";
                break;
            case 2:
                style = " class=\"silver_text_animated\" style=\"font-size: 29px; font-weight: bold;\"";
                break;
            case 3:
                style = " class=\"bronze_text_animated\" style=\"font-size: 28px;\"";
                break;
            default:
                style = " style=\"color:inherit;\"";
                break;
        }

		table.append(`<tr><td><span${style}>${(++i).toString()}</span></td><td><span${style}">${value["Id"].toString()}</span></td><td><span${style}">${value["Count"].toString()}</span></td></tr>`);
	}

	button.show();
}

async function update_user_records()
{
    let start = $("#user_records_start_date_input").val();
    let end = $("#user_records_end_date_input").val();

    if (start)
    {
        start = parseInt((new Date(start).getTime() / 1000).toFixed(0));
    }

    if (!start)
    {
        start = 0;
    }

    if (end)
    {
        end = parseInt((new Date(end).getTime() / 1000).toFixed(0));
    }

    if (!end)
    {
        end = 0x7FFFFFFF
    }

    let button = $("#user_records_table_update_button");
    button.hide();

    let result = await eel.py_get_user_records(start, end, 0x7FFFFFFF, 0)();
    if (typeof result !== "string")
    {
        button.show();
        switch (result)
        {
            case 0:
                swal("Connection error. Maybe server is offline.");
                return;
            case 401:
                swal("Invalid apikey")
                return;
            case 403:
                swal("Access restricted. Your account is banned?")
                return;
            case 503:
                swal("Internal server error")
                return;
            default:
                swal(`Request status code:${result}`);
                return;
        }
    }

    let records = JSON.parse(result);

    let table = $("#user_records_table");
    let tr = $("#user_records_table tr");
    for (let i = 1; i < tr.length;  i++)
    {
        tr[i].remove();
    }

	for (let value of records)
	{
		table.append(`<tr><td>-</td><td>${value["Id"].toString()}</td><td>${value["Count"].toString()}</td></tr>`);
	}

	button.show();
}

async function update_statistics()
{
    let start = $("#statistics_start_date_input").val();
    let end = $("#statistics_end_date_input").val();
    let take = $("#statistics_take_count_input").val();
    let skip = $("#statistics_skip_count_input").val();

    if (start)
    {
        start = parseInt((new Date(start).getTime() / 1000).toFixed(0));
    }

    if (!start)
    {
        start = 0;
    }

    if (end)
    {
        end = parseInt((new Date(end).getTime() / 1000).toFixed(0));
    }

    if (!end)
    {
        end = 0x7FFFFFFF
    }

    if (take)
    {
        take = parseInt(take);
    }

    if (!take)
    {
        take = 10;
    }

    if (skip)
    {
        skip = parseInt(skip);
    }

    if (!skip)
    {
        skip = 0;
    }

    let button = $("#statistics_table_update_button");
    button.hide();

    let result = await eel.py_get_statistics(start, end, take, skip)();
    if (typeof result !== "string")
    {
        button.show();
        switch (result)
        {
            case 0:
                swal("Connection error. Maybe server is offline.");
                return;
            case 401:
                swal("Invalid apikey")
                return;
            case 403:
                swal("Access restricted. Your account is banned?")
                return;
            case 503:
                swal("Internal server error")
                return;
            default:
                swal(`Request status code:${result}`);
                return;
        }
    }

    let records = JSON.parse(result);

    let table = $("#statistics_table");
    let tr = $("#statistics_table tr");
    for (let i = 1; i < tr.length;  i++)
    {
        tr[i].remove();
    }

	for (let value of records)
	{
		table.append(`<tr><td>${value["Id"].toString()}</td><td>${time_converter(value["Date"]).toString()}</td><td>${value["Count"].toString()}</td></tr>`);
	}

	button.show();
}

function format_date(date)
{
    let dt = new Date(date);
    let day = '' + dt.getDate();
    let month = '' + (dt.getMonth() + 1);
    let year = dt.getFullYear();

    if (day.length < 2)
    {
        day = '0' + day;
    }

    if (month.length < 2)
    {
        month = '0' + month;
    }

    return [day, month, year].join('-');
}

let chart;
async function update_graph()
{
    let start = $("#graph_start_date_input").val();
    let end = $("#graph_end_date_input").val();

    if (start)
    {
        start = parseInt((new Date(start).getTime() / 1000).toFixed(0));
    }

    if (!start)
    {
        start = 0;
    }

    if (end)
    {
        end = parseInt((new Date(end).getTime() / 1000).toFixed(0));
    }

    if (!end)
    {
        end = 0x7FFFFFFF
    }

    let button = $("#graph_table_update_button");
    button.hide();

    let result = await eel.py_get_statistics(start, end, 0x7FFFFFFF, 0)();
    if (typeof result !== "string")
    {
        button.show();
        switch (result)
        {
            case 0:
                swal("Connection error. Maybe server is offline.");
                return;
            case 401:
                swal("Invalid apikey")
                return;
            case 403:
                swal("Access restricted. Your account is banned?")
                return;
            case 503:
                swal("Internal server error")
                return;
            default:
                swal(`Request status code:${result}`);
                return;
        }
    }

	button.show();

    let records = JSON.parse(result);

    let data = {};
    let average = {};

    let count = 0;
    for (let item of records.reverse())
    {
        let date = format_date(item["Date"] * 1000);

        data[date] = count += item["Count"];
        average[date] = (average[date] === undefined ? 0 : average[date]) + item["Count"];
    }

    if (chart)
    {
        chart.destroy();
    }

    const canvas = $("#chart");
    chart = new Chart(canvas, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                data: data,
                label: "Summary",
                borderColor: "#FFFFFF",
                fill: false
            },
            {
                data: average,
                label: "Average",
                borderColor: "#7F7FFF",
                fill: false
            }]
        },
        legend: {
            display: false
        },
        options: {
            scales: {
                x: [{
                    type: 'time',
                    distribution: 'linear',
                }],
                title: {
                    display: false
                }
            },
            maintainAspectRatio: false,
            responsiveAnimationDuration: 1000
        }
    });
}

async function update_settings()
{
    let action_key_input = $("#settings_action_key");
    let collect_key_input = $("#settings_collect_key");

    let action_key = action_key_input.val();
    let collect_key = collect_key_input.val();

    if (action_key && action_key.length === 1 && collect_key && collect_key.length === 1)
    {
        eel.py_set_action_key(action_key);
        eel.py_set_collect_key(collect_key);
        log_notify("Settings successful saved", 1);
    }

    action_key_input.val(await eel.py_get_action_key()());
    collect_key_input.val(await eel.py_get_collect_key()());
}

async function show_page(offline=false)
{
	let tabs = $("#tabs");

	if (offline)
	{
	    $("#records_tab_link").remove();
	    $("#user_tab_link").remove();
	    $("#statistics_tab_link").remove();
	    $("#graph_tab_link").remove();
	    $("#records").remove();
	    $("#user").remove();
	    $("#statistics").remove();
	    $("#graph").remove();

	    await update_main();
	    await update_settings();

	    tabs.show();
	    return;
    }

	tabs.show();

	setInterval(update_records, 30000);
	setInterval(update_user_records, 30000);

	await update_main();
	await update_records();
	await update_user_records();
	await update_settings();
}

function open_tab(evt, classname)
{
    let tabcontent = document.getElementsByClassName("tabcontent");
    for (let i = 0; i < tabcontent.length; i++)
    {
        tabcontent[i].style.display = "none";
    }

    let tablinks = document.getElementsByClassName("tablinks");

    for (let i = 0; i < tablinks.length; i++)
    {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }

    document.getElementById(classname).style.display = "block";
    evt.currentTarget.className += " active";
}

document.getElementsByClassName("default_open")[0].click();