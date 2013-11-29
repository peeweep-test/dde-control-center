import QtQuick 2.1
import "calendar_core.js" as CalendarCore
import "../widgets/"

Item {
    id: calendarWidget
    width: 308
    property var clickedDateObject: new Date()
    property string clickedDate: CalendarCore.dateToString(clickedDateObject)
    property var cur_calendar;
    property var pre_calendar;
    property var next_calendar;
    property bool slideStop: true

    function monthChange(dateValue){
        var d = new Date(dateValue)
        if (d > cur_calendar.clickedDateObject && slideStop){
            //var next_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, 1)
            next_calendar = calendarSlideBox.createCanlendar(d, "next")
            if (!toNextMonth.running && !toPreviousMonth.running){
                toNextMonth.restart()
            }
            clickedDateObject = d
        }
        else if (d < cur_calendar.clickedDateObject && slideStop){
            //var pre_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, -1)
            pre_calendar = calendarSlideBox.createCanlendar(d, "previous")
            if (!toNextMonth.running && !toPreviousMonth.running){
                toPreviousMonth.restart()
            }
            clickedDateObject = d
        }
    }

    ParallelAnimation {
        id: toNextMonth

        onStarted: {
            slideStop = false
        }

        PropertyAnimation {
            target: cur_calendar
            properties: "x"
            to: calendarSlideBox.x - calendarSlideBox.width
            easing.type: Easing.InOutQuad 
            duration: 300
        }

        PropertyAnimation {
            target: next_calendar
            properties: "x"
            to: calendarSlideBox.x
            easing.type: Easing.InOutQuad 
            duration: 300
        }

        onStopped: {
            cur_calendar.destroy()
            cur_calendar = next_calendar
            slideStop = true

            //for (var i=0; i<cur_calendar.debug_dates.length; i++){
                //console.log(cur_calendar.debug_dates[i].dateValue)
            //}
        }
    }

    ParallelAnimation {
        id: toPreviousMonth

        onStarted: {
            slideStop = false
        }

        PropertyAnimation {
            target: pre_calendar
            properties: "x"
            to: calendarSlideBox.x
            easing.type: Easing.InOutQuad 
            duration: 300
        }

        PropertyAnimation {
            target: cur_calendar
            properties: "x"
            to: calendarSlideBox.x + calendarSlideBox.width
            easing.type: Easing.InOutQuad 
            duration: 300
        }

        onStopped: {
            cur_calendar.destroy()
            cur_calendar = pre_calendar
            slideStop = true

            //for (var i=0; i<cur_calendar.debug_dates.length; i++){
                //console.log(cur_calendar.debug_dates[i].dateValue)
            //}
        }
    }

    Rectangle {
        id: dateBoxAdjustment
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 28
        color: "#1a1b1b"

        Rectangle {
            //left border
            anchors.top: parent.top
            anchors.left: parent.left
            width: 1
            height: parent.height
            color: "#303132"
        }

        Rectangle {
            //left border
            anchors.top: parent.top
            anchors.right: parent.right
            width: 1
            height: parent.height
            color: "#120f10"
        }

        TextButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -1
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: dsTr("今天")

            onClicked: {
                monthChange(CalendarCore.dateToString(new Date()))
            }
        }

        ImageButton {
            id: decreaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: yearAdjustmentBox.left
            //anchors.leftMargin: 16

            nomralImage: 'images/arrow_left_normal.png'
            hoverImage: 'images/arrow_left_hover.png'
            pressImage: 'images/arrow_left_press.png'

            onClicked: { 
                var new_date_str = (yearAdjustment.yearNumber-1) + "-" + 
                    monthAdjustment.monthNumber + "-" + 1;
                //updateDates(new_date_str);
                monthChange(new_date_str)
            }
        }

        Rectangle{
            id: yearAdjustmentBox
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: increaseYearButton.left
            width: 50
            height: parent.height
            color: parent.color

            Text {
                id: yearAdjustment
                anchors.centerIn: parent
                color: textColor
                font.pixelSize: 12

                property int yearNumber: Number(clickedDateObject.getFullYear())

                text: yearNumber + "年"
            }
                
        }

        ImageButton {
            id: increaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: decreaseMonthButton.left
            anchors.rightMargin: 6

            nomralImage: 'images/arrow_right_normal.png'
            hoverImage: 'images/arrow_right_hover.png'
            pressImage: 'images/arrow_right_press.png'

            onClicked: { 
                var new_date_str = (yearAdjustment.yearNumber+1) + "-" + 
                    monthAdjustment.monthNumber + "-" + 1;
                //updateDates(new_date_str);
                monthChange(new_date_str)
            }
        }

        ImageButton {
            id: decreaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: monthAdjustmentBox.left

            nomralImage: 'images/arrow_left_normal.png'
            hoverImage: 'images/arrow_left_hover.png'
            pressImage: 'images/arrow_left_press.png'

            onClicked: { 
                if (monthAdjustment.monthNumber == 1) {
                    var new_monthNumber = 12
                    var new_yearNumber = yearAdjustment.yearNumber - 1
                }
                else {
                    var new_monthNumber = monthAdjustment.monthNumber - 1 
                    var new_yearNumber = yearAdjustment.yearNumber
                }
                var new_date_str = new_yearNumber + "-" + 
                    new_monthNumber + "-" + 1;
                //updateDates(new_date_str);
                monthChange(new_date_str)
            }
        }

        Rectangle{
            id: monthAdjustmentBox
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: increaseMonthButton.left
            width: 40
            height: parent.height
            color: parent.color

            Text {
                id: monthAdjustment
                anchors.centerIn: parent
                color: textColor
                font.pixelSize: 12

                property int monthNumber: Number(clickedDateObject.getMonth() + 1)
                text: monthNumber + "月"
            }
        }

        ImageButton {
            id: increaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 8

            nomralImage: 'images/arrow_right_normal.png'
            hoverImage: 'images/arrow_right_hover.png'
            pressImage: 'images/arrow_right_press.png'

            onClicked: {
                if (monthAdjustment.monthNumber == 12) {
                    var new_monthNumber = 1
                    var new_yearNumber = yearAdjustment.yearNumber + 1
                }
                else {
                    var new_monthNumber = monthAdjustment.monthNumber + 1
                    var new_yearNumber = yearAdjustment.yearNumber
                }
                var new_date_str = new_yearNumber + "-" + 
                    new_monthNumber + "-" + 1
                //updateDates(new_date_str)
                monthChange(new_date_str)
            }
        }
    }

    DSepratorHorizontal {
        anchors.top: dateBoxAdjustment.bottom
        width: parent.width
    }

    Rectangle {
        id: calendarSlideBox
        width: parent.width
        anchors.top: dateBoxAdjustment.bottom
        anchors.topMargin: 1
        anchors.left: parent.left

        Component.onCompleted: {
            var cur_d = clickedDateObject
            cur_calendar = createCanlendar(cur_d, '');
            var pre_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, -1)
            var next_d = CalendarCore.getDateWidthMonthStep(cur_calendar.clickedDateObject, 1)
            pre_calendar = createCanlendar(pre_d, "previous")
            next_calendar = createCanlendar(next_d, "next")
        }

        function createCanlendar(d_obj, position){
            var component = Qt.createComponent("CalendarComponent.qml");
            var calendar = component.createObject(calendarSlideBox, {
                "clickedDateObject": d_obj
            })

            if (position == 'previous'){
                calendar.x = calendarSlideBox.x - calendarSlideBox.width;
            }
            else if (position == 'next') {
                calendar.x = calendarSlideBox.x + calendarSlideBox.width;
            }
            else{
                calendar.x = calendarSlideBox.x
            }
            return calendar
        }
    }
}
